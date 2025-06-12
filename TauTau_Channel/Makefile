# BDT-Focused Analysis Makefile
# Simplified makefile for BDT analysis only

# Get ROOT compilation flags
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)
ROOTGLIBS  = $(shell root-config --glibs)

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -O2 $(ROOTCFLAGS)
LIBS = $(ROOTLIBS) -lTMVA -lTMVAGui -lTreePlayer

# =============================================================================
# BDT-RELATED TARGETS
# =============================================================================

# Core TMVA Tools
TMVA_TRAINING = TMVAClassification
TMVA_APPLICATION = TMVAClassificationApplication
BDT_OUTPUT = bdt_output
TMVA_DISTRIBUTIONS = TMVAOutPut_Distributions

# BDT Information Extraction Tools
INFO_BDT_BASE = Informacao_BDT_2018tautau
INFO_BDT_BSM70 = Informacao_BDT_2018tautau_bsm70
INFO_BDT_SCALAR400 = Informacao_BDT_2018tautau_scalar400
INFO_BDT_SCALAR600 = Informacao_BDT_2018tautau_scalar600
INFO_BDT_SCALAR800 = Informacao_BDT_2018tautau_scalar800
INFO_BDT_SCALAR1000 = Informacao_BDT_2018tautau_scalar1000
INFO_BDT_SCALAR1200 = Informacao_BDT_2018tautau_scalar1200
INFO_BDT_SCALAR1400 = Informacao_BDT_2018tautau_scalar1400

# BDT Plotting
PLOT_BDT_NORM = plot_bdt_norm

# Group all BDT targets
CORE_BDT_TARGETS = $(TMVA_TRAINING) $(TMVA_APPLICATION) $(BDT_OUTPUT) $(TMVA_DISTRIBUTIONS)
INFO_BDT_TARGETS = $(INFO_BDT_BASE) $(INFO_BDT_BSM70) $(INFO_BDT_SCALAR400) $(INFO_BDT_SCALAR600) $(INFO_BDT_SCALAR800) $(INFO_BDT_SCALAR1000) $(INFO_BDT_SCALAR1200) $(INFO_BDT_SCALAR1400)
PLOT_BDT_TARGETS = $(PLOT_BDT_NORM)

# All BDT targets
ALL_BDT_TARGETS = $(CORE_BDT_TARGETS) $(INFO_BDT_TARGETS) $(PLOT_BDT_TARGETS)

# =============================================================================
# BUILD RULES
# =============================================================================

# Default target - build all BDT tools
all: $(ALL_BDT_TARGETS)
	@echo "All BDT analysis tools compiled successfully!"

# Category-specific builds
core: $(CORE_BDT_TARGETS)
	@echo "Core BDT tools (TMVA + output processing) compiled!"

info: $(INFO_BDT_TARGETS)
	@echo "BDT information extraction tools compiled!"

plotting: $(PLOT_BDT_TARGETS)
	@echo "BDT plotting tools compiled!"

# Individual compilation rules
%: %.cpp
	@echo "Compiling $@..."
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

%: %.C
	@echo "Compiling $@..."
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

# =============================================================================
# BDT ANALYSIS PIPELINE
# =============================================================================

# Complete BDT analysis workflow
run-bdt-analysis: all
	@echo "=== STARTING BDT ANALYSIS PIPELINE ==="
	$(MAKE) run-training
	$(MAKE) run-application
	$(MAKE) run-bdt-output
	$(MAKE) run-bdt-info
	$(MAKE) run-bdt-plotting
	@echo "=== BDT ANALYSIS COMPLETE ==="

# Step 1: TMVA Training
run-training: $(TMVA_TRAINING)
	@echo "=== BDT TRAINING ==="
	./$(TMVA_TRAINING) 
	@echo "BDT training complete."

# Step 2: TMVA Application
run-application: $(TMVA_APPLICATION)
	@echo "=== BDT APPLICATION ==="
	./$(TMVA_APPLICATION) -t dy
	./$(TMVA_APPLICATION) -t ttjets
	./$(TMVA_APPLICATION) -t qcd
	./$(TMVA_APPLICATION) -t signal
	./$(TMVA_APPLICATION) -t data
	@echo "BDT application complete."

# Step 3: BDT Output Processing
run-bdt-output: $(BDT_OUTPUT) $(TMVA_DISTRIBUTIONS)
	@echo "=== BDT OUTPUT PROCESSING ==="
	./$(BDT_OUTPUT)
	./$(TMVA_DISTRIBUTIONS)
	@echo "BDT output processing complete."

# Step 4: BDT Information Extraction (All Mass Points)
run-bdt-info: $(INFO_BDT_TARGETS)
	@echo "=== BDT INFORMATION EXTRACTION ==="
	./$(INFO_BDT_BASE)
	./$(INFO_BDT_BSM70)
	./$(INFO_BDT_SCALAR400)
	./$(INFO_BDT_SCALAR600)
	./$(INFO_BDT_SCALAR800)
	./$(INFO_BDT_SCALAR1000)
	./$(INFO_BDT_SCALAR1200)
	./$(INFO_BDT_SCALAR1400)
	@echo "BDT information extraction complete."

# Step 5: BDT Plotting
run-bdt-plotting: $(PLOT_BDT_TARGETS)
	@echo "=== BDT PLOTTING ==="
	./$(PLOT_BDT_NORM)
	@echo "BDT plotting complete."

# =============================================================================
# QUICK RUNS AND TESTING
# =============================================================================

# Quick BDT test (training + application with one sample)
quick-bdt: $(TMVA_TRAINING) $(TMVA_APPLICATION)
	@echo "=== QUICK BDT TEST ==="
	./$(TMVA_TRAINING) -t standard -m BDT
	./$(TMVA_APPLICATION) -t dy
	@echo "Quick BDT test complete."

# Test specific mass point
test-scalar800: $(INFO_BDT_SCALAR800)
	./$(INFO_BDT_SCALAR800)

test-scalar1000: $(INFO_BDT_SCALAR1000)
	./$(INFO_BDT_SCALAR1000)

# Training only
train-only: $(TMVA_TRAINING)
	./$(TMVA_TRAINING) -t standard -m BDT

# Application only
apply-only: $(TMVA_APPLICATION)
	./$(TMVA_APPLICATION) -t signal

# BDT output only
output-only: $(BDT_OUTPUT)
	./$(BDT_OUTPUT)

# Plot only
plot-only: $(PLOT_BDT_TARGETS)
	./$(PLOT_BDT_NORM)

# =============================================================================
# DIFFERENT BDT CONFIGURATIONS
# =============================================================================

# Run BDT training with different methods
train-bdt-only: $(TMVA_TRAINING)
	./$(TMVA_TRAINING) -t standard -m BDT

train-multiple-methods: $(TMVA_TRAINING)
	./$(TMVA_TRAINING) -t standard -m BDT,MLP,Fisher,Likelihood

train-alternative-files: $(TMVA_TRAINING)
	./$(TMVA_TRAINING) -t alternative -m BDT

# Run all scalar mass points
run-all-scalars: $(INFO_BDT_SCALAR400) $(INFO_BDT_SCALAR600) $(INFO_BDT_SCALAR800) $(INFO_BDT_SCALAR1000) $(INFO_BDT_SCALAR1200) $(INFO_BDT_SCALAR1400)
	@echo "=== RUNNING ALL SCALAR MASS POINTS ==="
	./$(INFO_BDT_SCALAR400)
	./$(INFO_BDT_SCALAR600)
	./$(INFO_BDT_SCALAR800)
	./$(INFO_BDT_SCALAR1000)
	./$(INFO_BDT_SCALAR1200)
	./$(INFO_BDT_SCALAR1400)
	@echo "All scalar mass points complete."

# =============================================================================
# UTILITY TARGETS
# =============================================================================

# Development builds
debug: CXXFLAGS += -g -DDEBUG
debug: all

fast: CXXFLAGS = -O3 $(ROOTCFLAGS)
fast: all

# Check TMVA availability
check-tmva:
	@echo "Checking ROOT and TMVA installation..."
	@root-config --version
	@root-config --has-tmva
	@echo "Checking for TMVA libraries..."
	@root -l -b -q -e "gSystem->Load(\"libTMVA\")"

# Status check
status:
	@echo "=== BDT ANALYSIS STATUS ==="
	@echo "BDT executables built:"
	@ls -1 $(ALL_BDT_TARGETS) 2>/dev/null | wc -l | xargs echo "  Count:"
	@echo "TMVA output files:"
	@ls -1 *TMVA*.root 2>/dev/null | wc -l | xargs echo "  Count:"
	@echo "Dataset directories:"
	@ls -d dataset* 2>/dev/null | wc -l | xargs echo "  Count:"

# =============================================================================
# CLEANING TARGETS
# =============================================================================

clean:
	@echo "Cleaning all BDT executables and output files..."
	rm -f $(ALL_BDT_TARGETS)
	rm -rf dataset/
	rm -f *TMVA*.root
	rm -f *.log

clean-executables:
	@echo "Cleaning BDT executables only..."
	rm -f $(ALL_BDT_TARGETS)

clean-output:
	@echo "Cleaning BDT output files only..."
	rm -rf dataset/
	rm -f Output_TMVA*.root
	rm -f TMVApp*.root
	rm -f *.log

clean-training:
	@echo "Cleaning TMVA training outputs..."
	rm -rf dataset/
	rm -f Output_TMVA*.root

clean-application:
	@echo "Cleaning TMVA application outputs..."
	rm -f TMVApp*.root

# =============================================================================
# BACKUP AND HELP
# =============================================================================

backup:
	@echo "Creating BDT analysis backup..."
	tar -czf bdt_analysis_backup_$(shell date +%Y%m%d_%H%M).tar.gz \
		$(TMVA_TRAINING).C $(TMVA_APPLICATION).C \
		bdt_output.cpp TMVAOutPut_Distributions.cpp \
		Informacao_BDT_*.cpp plot_bdt_norm.cpp \
		Makefile

help:
	@echo "=== BDT ANALYSIS MAKEFILE HELP ==="
	@echo ""
	@echo "BUILD TARGETS:"
	@echo "  all              - Compile all BDT tools"
	@echo "  core             - Compile core TMVA tools only"
	@echo "  info             - Compile BDT information tools only"
	@echo "  plotting         - Compile BDT plotting tools only"
	@echo ""
	@echo "ANALYSIS PIPELINE:"
	@echo "  run-bdt-analysis     - Complete BDT workflow"
	@echo "  run-training         - TMVA training only"
	@echo "  run-application      - TMVA application only"
	@echo "  run-bdt-info         - BDT information extraction"
	@echo "  run-all-scalars      - All scalar mass points"
	@echo ""
	@echo "QUICK RUNS:"
	@echo "  quick-bdt            - Quick BDT test"
	@echo "  train-only           - Training only"
	@echo "  apply-only           - Application only"
	@echo "  output-only          - BDT output processing only"
	@echo "  plot-only            - Plotting only"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  train-bdt-only       - Train BDT method only"
	@echo "  train-multiple-methods - Train BDT+MLP+Fisher"
	@echo "  test-scalar800       - Test specific mass point"
	@echo ""
	@echo "UTILITIES:"
	@echo "  status           - Show BDT analysis status"
	@echo "  clean            - Remove all executables and outputs"
	@echo "  clean-output     - Remove only output files"
	@echo "  backup           - Create backup of BDT files"
	@echo "  debug            - Compile with debug flags"
	@echo "  check-tmva       - Check TMVA installation"
	@echo "  help             - Show this help"

.PHONY: all core info plotting run-bdt-analysis run-training run-application run-bdt-output run-bdt-info run-bdt-plotting quick-bdt test-scalar800 test-scalar1000 train-only apply-only output-only plot-only train-bdt-only train-multiple-methods train-alternative-files run-all-scalars debug fast check-tmva status clean clean-executables clean-output clean-training clean-application backup help