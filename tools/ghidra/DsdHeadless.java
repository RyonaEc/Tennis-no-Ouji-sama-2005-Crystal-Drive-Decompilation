//Headless dsd import + sync. Builds the dsd block layout from a ROM and then applies
//symbols, functions and relocations from a dsd config, with no GUI dialogs at all.
//@author Codebuff
//@category dsd
//@keybinding
//@menupath Tools.dsd headless import and sync
//@tooltip DsdHeadless.java <config.yaml> <rom.nds> [arm9|arm7] [dry]

import dsdghidra.DsdGhidra;
import dsdghidra.DsdGhidraPlugin;
import dsdghidra.loader.DsIoRegister;
import dsdghidra.loader.DsLoaderModule;
import dsdghidra.loader.DsMemoryRegion;
import dsdghidra.loader.DsRomLoaderData;
import dsdghidra.sync.*;
import dsdghidra.util.DsdError;
import ghidra.app.script.GhidraScript;
import ghidra.program.flatapi.FlatProgramAPI;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.BookmarkManager;
import ghidra.program.model.listing.BookmarkType;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.ProgramContext;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SymbolIterator;
import org.jetbrains.annotations.NotNull;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.nio.file.Files;
import java.nio.file.Paths;

/**
 * The two halves of the GUI workflow, without the dialogs.
 *
 * <p>The dsd loader asks ARM7/ARM9 with an {@code OptionDialogBuilder} and {@code SyncDsd}
 * asks for the config with a {@code GhidraFileChooser}, so neither can run under
 * {@code analyzeHeadless}. The dialog-free entry points they wrap are
 * {@code DsdGhidra.INSTANCE.get_loader_data} and {@code get_dsd_sync_data}, and this script
 * calls them directly: blocks come from the ROM exactly as the loader makes them
 * ({@code arm9_main}, {@code arm9_ov002}, {@code itcm}, {@code swram}, ... plus the IO
 * labels), then names, functions, thumb bits and relocation references come from the
 * dsd config. Run it against a freshly imported placeholder program; it removes whatever
 * blocks already exist before building the real layout.
 */
@SuppressWarnings("unused")
public class DsdHeadless extends GhidraScript {
    private boolean dryRun = false;
    private boolean arm7 = false;

    private Register thumbRegister;
    private DsModules dsModules;

    @Override
    public AnalysisMode getScriptAnalysisMode() {
        return AnalysisMode.SUSPENDED;
    }

    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            printerr("usage: DsdHeadless.java <config.yaml> <rom.nds> [arm9|arm7] [dry]");
            return;
        }
        String configPath = args[0];
        String romPath = args[1];
        if (args.length > 2) {
            this.arm7 = args[2].equalsIgnoreCase("arm7");
        }
        if (args.length > 3) {
            this.dryRun = args[3].equalsIgnoreCase("dry");
        }

        this.thumbRegister = currentProgram.getProgramContext().getRegister("TMode");

        int transaction = currentProgram.startTransaction("dsd headless import+sync");
        boolean commit = false;
        try {
            this.createBlocksFromRom(romPath);

            this.dsModules = new DsModules(currentProgram.getMemory());
            this.syncFromConfig(configPath);

            commit = !this.dryRun;
        } finally {
            currentProgram.endTransaction(transaction, commit);
        }

        this.printSummary();
    }

    /**
     * Same block layout as {@code DsRomLoader.load()}, minus the CPU dialog.
     */
    private void createBlocksFromRom(String romPath) throws Exception {
        DsdError dsdError = new DsdError();
        DsRomLoaderData data = new DsRomLoaderData();
        byte[] bytes = Files.readAllBytes(Paths.get(romPath));
        if (!DsdGhidra.INSTANCE.get_loader_data(bytes, bytes.length, data, dsdError.memory)) {
            String message = dsdError.getString();
            DsdGhidra.INSTANCE.free_error(dsdError.memory);
            throw new IOException("Failed to get ROM data from dsd-ghidra:\n\n" + message);
        }

        try {
            if (this.dryRun) {
                println("Dry run: not building memory blocks");
                return;
            }

            FlatProgramAPI api = new FlatProgramAPI(currentProgram, monitor);

            Memory memoryBefore = currentProgram.getMemory();
            for (MemoryBlock block : memoryBefore.getBlocks()) {
                try {
                    memoryBefore.removeBlock(block, monitor);
                } catch (Exception e) {
                    printerr("Could not remove block " + block.getName() + ": " + e.getMessage());
                }
            }

            if (this.arm7) {
                data.arm7.createBlock(api);
                for (DsLoaderModule overlay : data.getArm7Overlays()) {
                    overlay.createOverlayBlock(api);
                }
                for (DsMemoryRegion region : DsMemoryRegion.ARM7_REGIONS) {
                    region.createBlock(api);
                }
                for (DsIoRegister register : DsIoRegister.ARM7_REGS) {
                    api.createLabel(api.toAddr(register.address()), register.name(), true);
                }
            } else {
                data.arm9.createBlock(api);
                for (DsLoaderModule autoload : data.getAutoloads()) {
                    autoload.createBlock(api);
                }
                for (DsLoaderModule overlay : data.getArm9Overlays()) {
                    overlay.createOverlayBlock(api);
                }
                for (DsMemoryRegion region : DsMemoryRegion.ARM9_REGIONS) {
                    region.createBlock(api);
                }
                for (DsIoRegister register : DsIoRegister.ARM9_REGS) {
                    api.createLabel(api.toAddr(register.address()), register.name(), true);
                }
            }
            println("Built " + currentProgram.getMemory().getBlocks().length + " memory blocks");
        } finally {
            if (!DsdGhidra.INSTANCE.free_loader_data(data, dsdError.memory)) {
                printerr("Failed to free ROM data from dsd-ghidra");
            }
            DsdGhidra.INSTANCE.free_error(dsdError.memory);
        }
    }

    private void syncFromConfig(String configPath) throws Exception {
        DsdError dsdError = new DsdError();
        DsdSyncData dsdSyncData = new DsdSyncData();
        if (!DsdGhidra.INSTANCE.get_dsd_sync_data(configPath, dsdSyncData, dsdError.memory)) {
            String message = dsdError.getString();
            DsdGhidra.INSTANCE.free_error(dsdError.memory);
            throw new IOException("Failed to get sync data from dsd-ghidra:\n\n" + message);
        }

        try {
            this.doSync(dsdSyncData);
        } finally {
            if (!DsdGhidra.INSTANCE.free_dsd_sync_data(dsdSyncData, dsdError.memory)) {
                printerr("Failed to free sync data from dsd-ghidra");
            }
            DsdGhidra.INSTANCE.free_error(dsdError.memory);
        }
    }

    private void printSummary() {
        int blocks = currentProgram.getMemory().getBlocks().length;
        int functions = currentProgram.getFunctionManager().getFunctionCount();
        StringBuilder sample = new StringBuilder();
        FunctionIterator iterator = currentProgram.getFunctionManager().getFunctions(true);
        for (int i = 0; i < 5 && iterator.hasNext(); i++) {
            sample.append(' ').append(iterator.next().getName());
        }
        SymbolIterator named = currentProgram.getSymbolTable().getSymbols("func_020237f8");
        println("dsd headless: blocks=" + blocks + " functions=" + functions
            + " func_020237f8=" + (named.hasNext() ? "named" : "MISSING"));
        println("dsd headless: sample functions:" + sample);
    }

    private void doSync(DsdSyncData dsdSyncData) throws DsSection.Exception, DsModules.Exception {
        if (!this.dryRun) {
            this.removeBookmarks();
        }

        this.syncModule(dsdSyncData.arm9, this.dsModules.main);
        for (DsdSyncAutoload autoload : dsdSyncData.getAutoloads()) {
            switch (autoload.getKind()) {
                case Itcm -> this.syncModule(autoload.module, this.dsModules.itcm);
                case Dtcm -> this.syncModule(autoload.module, this.dsModules.dtcm);
                case Unknown -> {
                    DsModule dsModule = this.dsModules.getAutoload(autoload.index);
                    if (dsModule == null) {
                        printerr(String.format(
                            "No memory blocks for unknown autoload at %08x",
                            autoload.module.base_address
                        ));
                        return;
                    }
                    this.syncModule(autoload.module, dsModule);
                }
            }
        }
        for (DsdSyncOverlay overlay : dsdSyncData.getArm9Overlays()) {
            DsModule dsModule = this.dsModules.getOverlay(overlay.id);
            if (dsModule == null) {
                printerr(String.format("No memory blocks for overlay %d", overlay.id));
                return;
            }
            this.syncModule(overlay.module, dsModule);
        }
    }

    private void removeBookmarks() {
        BookmarkManager bookmarkManager = currentProgram.getBookmarkManager();
        BookmarkType sectionBookmark = DsdGhidraPlugin.getBookmarkTypeSection();
        BookmarkType delinkFileBookmark = DsdGhidraPlugin.getBookmarkTypeDelinkFile();

        if (sectionBookmark != null) {
            bookmarkManager.removeBookmarks(sectionBookmark.getTypeString());
        }
        if (delinkFileBookmark != null) {
            bookmarkManager.removeBookmarks(delinkFileBookmark.getTypeString());
        }
    }

    private void syncModule(DsdSyncModule dsdSyncModule, @NotNull DsModule dsModule)
        throws DsSection.Exception, DsModules.Exception {
        try {
            this.updateModule(dsdSyncModule, dsModule);
        } catch (Exception e) {
            printerr(String.format(
                "Failed to update module %s, see error:\n%s",
                dsModule.name,
                getExceptionStackTrace(e)
            ));
            return;
        }

        for (DsdSyncSection section : dsdSyncModule.getSections()) {
            if (section.base.isEmpty()) {
                continue;
            }

            DsSection dsSection;
            try {
                dsSection = dsModule.getRequiredSection(section.base);
            } catch (DsModule.Exception e) {
                printerr(e.toString());
                return;
            }

            this.updateSection(dsModule, dsSection);
            for (DsdSyncFunction function : section.getFunctions()) {
                this.updateFunction(function, dsSection);
            }
            for (DsdSyncDataSymbol dataSymbol : section.getSymbols()) {
                this.updateData(dataSymbol, dsSection);
            }
            for (DsdSyncRelocation relocation : section.getRelocations()) {
                this.updateReferences(relocation, dsSection);
            }
        }
        for (DsdSyncDelinkFile file : dsdSyncModule.getFiles()) {
            this.updateDelinkFile(file, dsModule);
        }
    }

    private void updateModule(DsdSyncModule module, DsModule dsModule) {
        SyncModule syncModule = new SyncModule(currentProgram, module, dsModule);

        if (syncModule.needsUpdate()) {
            this.println("Updating sections in module '" + dsModule.name + "'");
            if (!this.dryRun) {
                try {
                    syncModule.join();
                } catch (Exception e) {
                    printerr(String.format(
                        "Failed to join module %s, see error:\n%s",
                        dsModule.name,
                        getExceptionStackTrace(e)
                    ));
                    return;
                }
                try {
                    syncModule.split();
                } catch (Exception e) {
                    printerr(String.format(
                        "Failed to split module %s, see error:\n%s",
                        dsModule.name,
                        getExceptionStackTrace(e)
                    ));
                    return;
                }
            }
        }
    }

    private void updateSection(DsModule dsModule, DsSection dsSection) {
        SyncSection syncSection = new SyncSection(currentProgram, dsSection, dsModule);

        if (!this.dryRun) {
            syncSection.addBookmark();
        }
    }

    private void updateDelinkFile(DsdSyncDelinkFile delinkFile, DsModule dsModule) {
        SyncDelinkFile syncDelinkFile = new SyncDelinkFile(currentProgram, delinkFile, dsModule);
        if (!this.dryRun) {
            try {
                syncDelinkFile.addBookmarks();
            } catch (Exception e) {
                printerr(String.format(
                    "Failed to add bookmarks for delink files in %s",
                    dsModule.name
                ));
            }
        }
    }

    private void updateFunction(DsdSyncFunction function, DsSection dsSection) {
        SyncFunction syncFunction;
        try {
            syncFunction = new SyncFunction(currentProgram, dsSection, function);
        } catch (Exception e) {
            printerr(String.format(
                "Failed to get function '%s' at %08x in %s, see error:\n%s",
                function.name.getString(),
                function.start,
                dsSection.getModule().name,
                getExceptionStackTrace(e)
            ));
            return;
        }

        Function ghidraFunction = syncFunction.getExistingGhidraFunction();
        if (ghidraFunction == null) {
            String mode = function.thumb ? "thumb" : "arm";
            println("Adding function " + syncFunction.symbolName.symbol + " (" + mode + ") at " + syncFunction.start);

            if (!this.dryRun) {
                try {
                    ghidraFunction = syncFunction.createGhidraFunction(monitor);
                } catch (Exception e) {
                    printerr(String.format(
                        "Failed to create Ghidra function '%s' at %s in %s, see error:\n%s",
                        syncFunction.symbolName.name,
                        syncFunction.start,
                        dsSection.getModule().name,
                        getExceptionStackTrace(e)
                    ));
                    return;
                }
            }
        } else if (syncFunction.ghidraFunctionNeedsUpdate(ghidraFunction)) {
            println("Updating function " + syncFunction.symbolName.symbol + " at " + syncFunction.start);
            if (!this.dryRun) {
                try {
                    syncFunction.updateGhidraFunction(ghidraFunction);
                } catch (Exception e) {
                    printerr(String.format(
                        "Failed to update Ghidra function '%s' at %s in %s, see error:\n%s",
                        ghidraFunction.getName(),
                        syncFunction.start,
                        dsSection.getModule().name,
                        getExceptionStackTrace(e)
                    ));
                    return;
                }
            }
        }

        if (!this.dryRun) {
            try {
                syncFunction.definePoolConstants(this);
            } catch (Exception e) {
                printerr(String.format(
                    "Failed to define pool constants in function '%s' at %s in %s, see error:\n%s",
                    ghidraFunction.getName(),
                    syncFunction.start,
                    dsSection.getModule().name,
                    getExceptionStackTrace(e)
                ));
                return;
            }
            syncFunction.disassemble(this.thumbRegister, monitor);
            syncFunction.referPoolConstants(this);
        }
    }

    private void updateData(DsdSyncDataSymbol dataSymbol, DsSection dsSection) {
        SyncDataSymbol syncDataSymbol;
        try {
            syncDataSymbol = new SyncDataSymbol(currentProgram, dsSection, dataSymbol);
        } catch (Exception e) {
            printerr(String.format(
                "Failed to get data symbol '%s' at %08x in %s of %s, see error:\n%s",
                dataSymbol.name.getString(),
                dataSymbol.address,
                dsSection.getName(),
                dsSection.getModule().name,
                getExceptionStackTrace(e)
            ));
            return;
        }

        boolean needsUpdate = syncDataSymbol.checkNeedsUpdate();
        String currentName = syncDataSymbol.getCurrentLabel();
        boolean exists = currentName != null;

        if (exists) {
            if (needsUpdate) {
                if (!this.dryRun) {
                    syncDataSymbol.deleteExistingLabels();
                }
                println("Updating data " + currentName + " at " + syncDataSymbol.address + " to name " + syncDataSymbol.symbolName.symbol);
            } else {
                return;
            }
        } else {
            println("Adding data " + syncDataSymbol.symbolName.symbol + " at " + syncDataSymbol.address);
        }

        if (!this.dryRun) {
            try {
                syncDataSymbol.createLabel();
            } catch (Exception e) {
                printerr(String.format(
                    "Failed to create data symbol label '%s' at %s in section %s of %s, see error:\n%s",
                    syncDataSymbol.symbolName.name,
                    syncDataSymbol.address,
                    dsSection.getName(),
                    dsSection.getModule().name,
                    getExceptionStackTrace(e)
                ));
            }
            syncDataSymbol.defineData(this);
        }
    }

    private void updateReferences(DsdSyncRelocation relocation, DsSection dsSection) {
        SyncRelocation syncRelocation;
        try {
            syncRelocation = new SyncRelocation(currentProgram, dsSection, relocation);
        } catch (DsSection.Exception e) {
            printerr(String.format(
                "Failed to get relocation from %08x in section %s of %s, see error:\n%s",
                relocation.from,
                dsSection.getName(),
                dsSection.getModule().name,
                getExceptionStackTrace(e)
            ));
            return;
        }

        String updateReason = syncRelocation.getUpdateReason();
        if (updateReason != null) {
            println("Updating references from " + syncRelocation.from + " : " + updateReason);
            if (!this.dryRun) {
                syncRelocation.deleteExistingReferences();
            }
        } else if (!syncRelocation.existsInGhidra()) {
            println("Adding references from " + syncRelocation.from);
        } else {
            return;
        }

        if (!this.dryRun) {
            try {
                syncRelocation.addReferences(this, this.dsModules);
            } catch (Exception e) {
                printerr(String.format(
                    "Failed to add reference from %08x in section %s of %s, see error:\n%s",
                    relocation.from,
                    dsSection.getName(),
                    dsSection.getModule().name,
                    getExceptionStackTrace(e)
                ));
            }
        }
    }

    public static String getExceptionStackTrace(Exception e) {
        StringWriter writer = new StringWriter();
        e.printStackTrace(new PrintWriter(writer));
        return writer.toString();
    }
}
