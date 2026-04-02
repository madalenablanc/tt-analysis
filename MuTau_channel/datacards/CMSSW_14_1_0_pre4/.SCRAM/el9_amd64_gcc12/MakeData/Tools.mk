ALL_TOOLS      += abseil-cpp
abseil-cpp_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/abseil-cpp/20220623.1-237da70ecfddbd80ec9c94531c1cbba7/include
abseil-cpp_EX_LIB := absl_bad_any_cast_impl absl_bad_optional_access absl_bad_variant_access absl_base absl_city absl_civil_time absl_cord absl_debugging_internal absl_demangle_internal absl_examine_stack absl_exponential_biased absl_failure_signal_handler absl_flags_commandlineflag_internal absl_flags_commandlineflag absl_flags_config absl_flags_internal absl_flags_marshalling absl_flags_parse absl_flags_private_handle_accessor absl_flags_program_name absl_flags_reflection absl_flags absl_flags_usage_internal absl_flags_usage absl_graphcycles_internal absl_hash absl_hashtablez_sampler absl_int128 absl_leak_check absl_log_severity absl_malloc_internal absl_periodic_sampler absl_random_distributions absl_random_internal_distribution_test_util absl_random_internal_platform absl_random_internal_pool_urbg absl_random_internal_randen_hwaes_impl absl_random_internal_randen_hwaes absl_random_internal_randen_slow absl_random_internal_randen absl_random_internal_seed_material absl_random_seed_gen_exception absl_random_seed_sequences absl_raw_hash_set absl_raw_logging_internal absl_scoped_set_env absl_spinlock_wait absl_stacktrace absl_statusor absl_status absl_strerror absl_str_format_internal absl_strings_internal absl_strings absl_symbolize absl_synchronization absl_throw_delegate absl_time absl_time_zone

ALL_TOOLS      += alpaka-cuda
alpaka-cuda_EX_USE := alpaka cuda
alpaka-cuda_EX_FLAGS_CUDA_FLAGS  := -DALPAKA_ACC_GPU_CUDA_ENABLED -DALPAKA_ACC_GPU_CUDA_ONLY_MODE -UALPAKA_HOST_ONLY
alpaka-cuda_EX_FLAGS_CXXFLAGS  := -DALPAKA_ACC_GPU_CUDA_ENABLED -DALPAKA_ACC_GPU_CUDA_ONLY_MODE -DALPAKA_HOST_ONLY
alpaka-cuda_EX_FLAGS_GENREFLEX_CPPFLAGS  := -DALPAKA_ACC_GPU_CUDA_ENABLED -DALPAKA_ACC_GPU_CUDA_ONLY_MODE -DALPAKA_HOST_ONLY

ALL_TOOLS      += alpaka-rocm
alpaka-rocm_EX_USE := alpaka rocm
alpaka-rocm_EX_FLAGS_CXXFLAGS  := -DALPAKA_ACC_GPU_HIP_ENABLED -DALPAKA_ACC_GPU_HIP_ONLY_MODE -DALPAKA_HOST_ONLY
alpaka-rocm_EX_FLAGS_GENREFLEX_CPPFLAGS  := -DALPAKA_ACC_GPU_HIP_ENABLED -DALPAKA_ACC_GPU_HIP_ONLY_MODE -DALPAKA_HOST_ONLY
alpaka-rocm_EX_FLAGS_ROCM_FLAGS  := -DALPAKA_ACC_GPU_HIP_ENABLED -DALPAKA_ACC_GPU_HIP_ONLY_MODE -UALPAKA_HOST_ONLY

ALL_TOOLS      += alpaka-serial
alpaka-serial_EX_USE := alpaka
alpaka-serial_EX_FLAGS_CXXFLAGS  := -DALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED
alpaka-serial_EX_FLAGS_GENREFLEX_CPPFLAGS  := -DALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED

ALL_TOOLS      += alpaka-tbb
alpaka-tbb_EX_USE := alpaka tbb
alpaka-tbb_EX_FLAGS_CXXFLAGS  := -DALPAKA_ACC_CPU_B_TBB_T_SEQ_ENABLED
alpaka-tbb_EX_FLAGS_GENREFLEX_CPPFLAGS  := -DALPAKA_ACC_CPU_B_TBB_T_SEQ_ENABLED

ALL_TOOLS      += alpaka
alpaka_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/alpaka/1.1.0-fdf91b0de01f1f1b6a46ba72110f09a6/include
alpaka_EX_USE := boost
alpaka_EX_FLAGS_CUDA_FLAGS  := -DALPAKA_DEFAULT_HOST_MEMORY_ALIGNMENT=128 -DALPAKA_DISABLE_VENDOR_RNG
alpaka_EX_FLAGS_CXXFLAGS  := -DALPAKA_DEFAULT_HOST_MEMORY_ALIGNMENT=128 -DALPAKA_DISABLE_VENDOR_RNG
alpaka_EX_FLAGS_GENREFLEX_CPPFLAGS  := -DALPAKA_DEFAULT_HOST_MEMORY_ALIGNMENT=128 -DALPAKA_DISABLE_VENDOR_RNG

ALL_TOOLS      += alpgen

ALL_TOOLS      += axol1tl

ALL_TOOLS      += blackhat
blackhat_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/blackhat/0.9.9-4be91c0829dd8aea91e82a42dbf0e29e/include
blackhat_EX_LIB := Ampl_eval BG BH BHcore CutPart Cut_wCI Cuteval Integrals Interface OLA RatPart Rateval Spinors assembly ratext
blackhat_EX_USE := qd

ALL_TOOLS      += boost
boost_EX_LIB := boost_thread boost_date_time
boost_EX_USE := boost_header

ALL_TOOLS      += boost_chrono
boost_chrono_EX_LIB := boost_chrono
boost_chrono_EX_USE := boost_system boost

ALL_TOOLS      += boost_filesystem
boost_filesystem_EX_LIB := boost_filesystem
boost_filesystem_EX_USE := boost_system boost

ALL_TOOLS      += boost_header
boost_header_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/boost/1.80.0-87b5de10acd2f2c8a325345ad058b814/include
boost_header_EX_USE := sockets root_cxxdefaults
boost_header_EX_FLAGS_CPPDEFINES  := -DBOOST_SPIRIT_THREADSAFE -DPHOENIX_THREADSAFE -DBOOST_MATH_DISABLE_STD_FPCLASSIFY -DBOOST_UUID_RANDOM_PROVIDER_FORCE_POSIX
boost_header_EX_FLAGS_CXXFLAGS  := -Wno-error=unused-variable
boost_header_EX_FLAGS_SYSTEM_INCLUDE  := 1

ALL_TOOLS      += boost_iostreams
boost_iostreams_EX_LIB := boost_iostreams
boost_iostreams_EX_USE := boost

ALL_TOOLS      += boost_mpi
boost_mpi_EX_LIB := boost_mpi
boost_mpi_EX_USE := boost boost_serialization

ALL_TOOLS      += boost_program_options
boost_program_options_EX_LIB := boost_program_options
boost_program_options_EX_USE := boost

ALL_TOOLS      += boost_python
boost_python_EX_LIB := boost_python39
boost_python_EX_USE := boost_header python3

ALL_TOOLS      += boost_regex
boost_regex_EX_LIB := boost_regex
boost_regex_EX_USE := boost

ALL_TOOLS      += boost_serialization
boost_serialization_EX_LIB := boost_serialization
boost_serialization_EX_USE := boost

ALL_TOOLS      += boost_system
boost_system_EX_LIB := boost_system
boost_system_EX_USE := boost

ALL_TOOLS      += boost_test
boost_test_EX_LIB := boost_unit_test_framework
boost_test_EX_USE := boost

ALL_TOOLS      += bz2lib
bz2lib_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/bz2lib/1.0.6-d065ccd79984efc6d4660f410e4c81de/include
bz2lib_EX_LIB := bz2
bz2lib_EX_USE := root_cxxdefaults

ALL_TOOLS      += c-ares
c-ares_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/c-ares/1.15.0-bf0e2c49330266ca228085a1f6d7a564/include
c-ares_EX_LIB := cares

ALL_TOOLS      += catch2
catch2_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/catch2/2.13.6-17102db92de47c6a473c6e67627c548a/include

ALL_TOOLS      += celeritas
celeritas_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/celeritas/v0.4.1-3f0e1a60904ed85d33c4fb5635eca3ee/include
celeritas_EX_LIB := accel celeritas corecel
celeritas_EX_USE := vecgeom_interface geant4core vecgeom
celeritas_EX_FLAGS_REM_CXXFLAGS  := -Werror=missing-braces

ALL_TOOLS      += cepgen
cepgen_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/cepgen/1.2.3-937c0a11df5b9a0d612efc8a9436e5ac/include
cepgen_EX_LIB := CepGen CepGenHepMC2 CepGenHepMC3 CepGenLHAPDF CepGenProcesses CepGenPythia6
cepgen_EX_USE := gsl openblas hepmc hepmc3 lhapdf pythia6

ALL_TOOLS      += cicada

ALL_TOOLS      += clang-uml
clang-uml_EX_USE := yaml-cpp zlib

ALL_TOOLS      += classlib
classlib_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/classlib/3.1.3-c6b100380ee001a2bc06975bb0682ffc/include
classlib_EX_LIB := classlib
classlib_EX_USE := pcre root_cxxdefaults
classlib_EX_FLAGS_CPPDEFINES  := -D__STDC_LIMIT_MACROS -D__STDC_FORMAT_MACROS

ALL_TOOLS      += clhep
clhep_EX_LIB := CLHEP
clhep_EX_USE := clhepheader

ALL_TOOLS      += clhepheader
clhepheader_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/clhep/2.4.7.1-3f4b98e6a48a97b8e585febf5b6a5dbb/include
clhepheader_EX_USE := root_cxxdefaults
clhepheader_EX_FLAGS_ROOTCLING_ARGS  := -moduleMapFile=$(CLHEP_BASE)/include/module.modulemap
clhepheader_EX_FLAGS_SYSTEM_INCLUDE  := 1

ALL_TOOLS      += cmsswdata
cmsswdata_EX_FLAGS_CMSSW_DATA_PACKAGE  := Alignment/OfflineValidation=V00-03-00 CalibCalorimetry/CaloMiscalibTools=V01-00-00 CalibCalorimetry/EcalTrivialCondModules=V00-03-00 CalibPPS/ESProducers=V01-04-00 CalibTracker/SiPixelESProducers=V02-03-00 CalibTracker/SiStripDCS=V01-01-00 Calibration/Tools=V01-00-00 CondFormats/JetMETObjects=V01-00-03 CondTools/Hcal=V00-01-00 CondTools/SiPhase2Tracker=V00-02-00 CondTools/SiStrip=V00-02-00 Configuration/Generator=V01-08-00 DQM/DTMonitorClient=V00-01-00 DQM/EcalMonitorClient=V00-03-00 DQM/HcalTasks=V00-01-00 DQM/Integration=V00-06-00 DQM/PhysicsHWW=V01-00-00 DQM/SiStripMonitorClient=V01-01-00 DataFormats/Common=V00-01-00 DataFormats/DetId=V00-01-00 DataFormats/FEDRawData=V00-01-00 DataFormats/HLTReco=V00-01-00 DataFormats/L1Scouting=V00-02-00 DataFormats/L1ScoutingRawData=V00-02-00 DataFormats/L1TGlobal=V00-01-00 DataFormats/PatCandidates=V01-01-00 DataFormats/Scouting=V00-03-00 DataFormats/SiStripCluster=V00-01-00 DetectorDescription/Schema=V02-03-00 EgammaAnalysis/ElectronTools=V00-03-01 EventFilter/L1TRawToDigi=V01-00-00 FWCore/Modules=V00-01-00 FastSimulation/MaterialEffects=V05-00-00 FastSimulation/TrackingRecHitProducer=V01-00-03 Fireworks/Geometry=V07-06-00 GeneratorInterface/EvtGenInterface=V02-09-00 GeneratorInterface/ReggeGribovPartonMCInterface=V00-00-02 Geometry/DTGeometryBuilder=V00-01-00 Geometry/HGCalMapping=V00-01-00 Geometry/TestReference=V00-12-00 HLTrigger/JetMET=V01-00-00 HeterogeneousCore/SonicTriton=V00-03-00 IOPool/Input=V00-03-00 L1Trigger/CSCTriggerPrimitives=V00-12-00 L1Trigger/DTTriggerPhase2=V00-03-00 L1Trigger/L1CaloTrigger=V00-01-00 L1Trigger/L1TCalorimeter=V01-02-00 L1Trigger/L1TGlobal=V00-04-00 L1Trigger/L1THGCal=V01-08-00 L1Trigger/L1TMuon=V01-11-00 L1Trigger/L1TMuonEndCapPhase2=V00-01-00 L1Trigger/L1TTrackMatch=V00-01-00 L1Trigger/Phase2L1ParticleFlow=V00-07-00 L1Trigger/RPCTrigger=V00-15-00 L1Trigger/TrackFindingTMTT=V00-02-00 L1Trigger/TrackFindingTracklet=V00-03-00 L1Trigger/TrackTrigger=V00-03-00 L1Trigger/VertexFinder=V00-01-00 L1TriggerConfig/L1TConfigProducers=V00-01-00 MagneticField/Engine=V00-01-00 MagneticField/Interpolation=V01-02-00 PhysicsTools/NanoAOD=V01-04-00 PhysicsTools/PatUtils=V00-05-00 RecoBTag/CTagging=V01-00-03 RecoBTag/Combined=V01-22-00 RecoBTag/SecondaryVertex=V02-00-04 RecoBTag/SoftLepton=V01-00-01 RecoCTPPS/TotemRPLocal=V00-02-00 RecoEcal/EgammaClusterProducers=V00-04-00 RecoEgamma/EgammaPhotonProducers=V00-05-00 RecoEgamma/ElectronIdentification=V01-14-00 RecoEgamma/PhotonIdentification=V01-08-00 RecoHGCal/TICL=V00-03-00 RecoHI/HiJetAlgos=V01-00-01 RecoJets/JetProducers=V05-14-00 RecoLocalCalo/EcalDeadChannelRecoveryAlgos=V01-01-00 RecoMET/METPUSubtraction=V01-03-00 RecoMTD/TimingIDTools=V00-01-00 RecoMuon/MuonIdentification=V01-16-00 RecoMuon/TrackerSeedGenerator=V00-06-00 RecoPPS/Local=V00-01-00 RecoParticleFlow/PFBlockProducer=V02-04-02 RecoParticleFlow/PFProducer=V16-02-00 RecoParticleFlow/PFTracking=V13-01-00 RecoTauTag/TrainingFiles=V00-09-00 RecoTracker/DisplacedRegionalTracking=V00-01-00 RecoTracker/FinalTrackSelectors=V01-05-00 RecoTracker/MkFit=V00-14-00 RecoTracker/TkSeedGenerator=V00-03-00 SLHCUpgradeSimulations/Geometry=V01-00-10 SimCalorimetry/EcalEBTrigPrimProducers=V00-01-00 SimG4CMS/Calo=V03-05-00 SimG4CMS/Forward=V02-04-00 SimG4CMS/HGCalTestBeam=V01-00-00 SimPPS/PPSPixelDigiProducer=V00-00-02 SimTracker/SiStripDigitizer=V01-01-00 SimTransport/HectorProducer=V01-00-01 SimTransport/PPSProtonTransport=V00-02-00 SimTransport/TotemRPProtonTransportParametrization=V00-01-00 Validation/Geometry=V00-07-00 Validation/HGCalValidation=V00-06-00

ALL_TOOLS      += conifer
conifer_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/conifer/1.3-bcc2ed9f9f2754550d89b349b9321f33/include
conifer_EX_USE := json

ALL_TOOLS      += coral
ALL_SCRAM_PROJECTS += coral
coral_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/coral/CORAL_2_3_21-76a432200e6de76d03247e347c630e51/include/LCG
coral_EX_USE := root_cxxdefaults
coral_ORDER := 98000

ALL_TOOLS      += correctionlib
correctionlib_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/py3-correctionlib/2.5.0-dd3b649f928a80ad844fe7326f1f2c9d/lib/python3.9/site-packages/correctionlib/include
correctionlib_EX_LIB := correctionlib

ALL_TOOLS      += cppunit
cppunit_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/cppunit/1.15.x-9c5e10ac1b9ff043c0e3d26ccceb0058/include
cppunit_EX_LIB := cppunit
cppunit_EX_USE := root_cxxdefaults sockets

ALL_TOOLS      += cpu_features
cpu_features_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/cpu_features/0.7.0-7666a8767a46b2bd20b226b315f46315/include
cpu_features_EX_LIB := cpu_features

ALL_TOOLS      += csctrackfinderemulation
csctrackfinderemulation_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/CSCTrackFinderEmulation/1.2-4d8fc45bae4641b9aeb067b33fc0e482/include
csctrackfinderemulation_EX_LIB := CSCTrackFinderEmulation

ALL_TOOLS      += cuda-compatible-runtime
cuda-compatible-runtime_EX_USE := cuda cuda-stubs

ALL_TOOLS      += cuda-cublas
cuda-cublas_EX_LIB := cublas cublasLt
cuda-cublas_EX_USE := cuda

ALL_TOOLS      += cuda-cufft
cuda-cufft_EX_LIB := cufft cufftw
cuda-cufft_EX_USE := cuda

ALL_TOOLS      += cuda-curand
cuda-curand_EX_LIB := curand
cuda-curand_EX_USE := cuda

ALL_TOOLS      += cuda-cusolver
cuda-cusolver_EX_LIB := cusolver cusolverMg
cuda-cusolver_EX_USE := cuda

ALL_TOOLS      += cuda-cusparse
cuda-cusparse_EX_LIB := cusparse
cuda-cusparse_EX_USE := cuda

ALL_TOOLS      += cuda-gcc-support

ALL_TOOLS      += cuda-npp
cuda-npp_EX_LIB := nppial nppicc nppidei nppif nppig nppim nppist nppisu nppitc npps nppc
cuda-npp_EX_USE := cuda

ALL_TOOLS      += cuda-nvgraph
cuda-nvgraph_EX_LIB := nvgraph
cuda-nvgraph_EX_USE := cuda

ALL_TOOLS      += cuda-nvjpeg
cuda-nvjpeg_EX_LIB := nvjpeg
cuda-nvjpeg_EX_USE := cuda

ALL_TOOLS      += cuda-nvml
cuda-nvml_EX_LIB := nvidia-ml
cuda-nvml_EX_USE := cuda-stubs

ALL_TOOLS      += cuda-nvrtc
cuda-nvrtc_EX_LIB := nvrtc
cuda-nvrtc_EX_USE := cuda

ALL_TOOLS      += cuda-stubs
cuda-stubs_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/cuda/12.4.1-8b491273511f117bf853a3f82dc2d34f/include
cuda-stubs_EX_LIB := cuda
cuda-stubs_EX_LIBDIR := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/cuda/12.4.1-8b491273511f117bf853a3f82dc2d34f/lib64/stubs
cuda-stubs_EX_FLAGS_SKIP_TOOL_SYMLINKS  := 1

ALL_TOOLS      += cuda
ALL_LIB_TYPES += CUDA_LIB
cuda_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/cuda/12.4.1-8b491273511f117bf853a3f82dc2d34f/include
cuda_EX_LIB := cudart cudadevrt nvToolsExt
cuda_EX_CUDA_LIB := cudadevrt
cuda_EX_USE := cuda-stubs
cuda_EX_FLAGS_CUDA_FLAGS  := -std=c++17 -O3 --generate-line-info --source-in-ptx --display-error-number --expt-relaxed-constexpr --extended-lambda -gencode arch=compute_60,code=[sm_60,compute_60] -gencode arch=compute_70,code=[sm_70,compute_70] -gencode arch=compute_75,code=[sm_75,compute_75] -gencode arch=compute_80,code=[sm_80,compute_80] -gencode arch=compute_89,code=[sm_89,compute_89] -Wno-deprecated-gpu-targets -Xcudafe --diag_suppress=esa_on_defaulted_function_ignored -Xcudafe --gnu_version=120300 --cudart shared
cuda_EX_FLAGS_CUDA_HOST_CXXFLAGS  := -std=c++17
cuda_EX_FLAGS_REM_CUDA_HOST_CXXFLAGS  := -std=% %potentially-evaluated-expression

ALL_TOOLS      += cudnn
cudnn_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/cudnn/8.9.7.29-e2a20f8f557267f79cfd0afa8ccc0b72/include
cudnn_EX_LIB := cudnn
cudnn_EX_USE := cuda

ALL_TOOLS      += cupti
cupti_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/cuda/12.4.1-8b491273511f117bf853a3f82dc2d34f/include
cupti_EX_LIB := cupti

ALL_TOOLS      += curl
curl_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/curl/7.79.0-e9aea8dd47e409f0dcfd76a7b3220112/include
curl_EX_LIB := curl
curl_EX_USE := root_cxxdefaults

ALL_TOOLS      += dablooms
dablooms_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/dablooms/0.9.1-6a52f3a6ac2cb636047ac50c3673033a/include
dablooms_EX_LIB := dablooms

ALL_TOOLS      += das_client

ALL_TOOLS      += davix
davix_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/davix/0.8.1-c154730cbbe4a609cedbfd2e95b3b6de/include/davix
davix_EX_LIB := davix
davix_EX_USE := boost_system libxml2

ALL_TOOLS      += db6
db6_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/db6/6.2.32-0b5a52d56859c2628499360eaabc790e/include
db6_EX_LIB := db

ALL_TOOLS      += dcap
dcap_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/dcap/2.47.12-6fedb51a2b7f6cc676419dddc72c874d/include
dcap_EX_LIB := dcap
dcap_EX_USE := root_cxxdefaults

ALL_TOOLS      += dd4hep-core
dd4hep-core_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/dd4hep/v01-27-02-39caf811614f278f42bf52fb83e7c15c/include
dd4hep-core_EX_LIB := DDCore DDParsers
dd4hep-core_EX_USE := root_cxxdefaults root boost xerces-c clhep
dd4hep-core_EX_FLAGS_CPPDEFINES  := -DDD4HEP_USE_GEANT4_UNITS=1
dd4hep-core_EX_FLAGS_LISTCOMPONENTS  := $(DD4HEP_CORE_BASE)/bin/listcomponents_dd4hep

ALL_TOOLS      += dd4hep-geant4
dd4hep-geant4_EX_LIB := DDG4-static
dd4hep-geant4_EX_USE := geant4core dd4hep-core

ALL_TOOLS      += dd4hep
dd4hep_EX_LIB := DDAlign DDCond
dd4hep_EX_USE := dd4hep-core

ALL_TOOLS      += dip-platform-dependent
dip-platform-dependent_EX_LIB := platform-dependent
dip-platform-dependent_EX_USE := dip_interface

ALL_TOOLS      += dip
dip_EX_LIB := dip
dip_EX_USE := dip-platform-dependent log4cplus

ALL_TOOLS      += dip_interface
dip_interface_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/dip/8693f00cc422b4a15858fcd84249acaeb07b6316-4778d17af7593171e1194c6574290222/include

ALL_TOOLS      += eigen
eigen_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/eigen/3bb6a48d8c171cf20b5f8e48bfb4e424fbd4f79e-3ca740c03e68b1a067f3ed0679234a78/include/eigen3
eigen_EX_FLAGS_CUDA_FLAGS  := --diag-suppress 20014
eigen_EX_FLAGS_CXXFLAGS  := -DEIGEN_DONT_PARALLELIZE -DEIGEN_MAX_ALIGN_BYTES=64

ALL_TOOLS      += evtgen
evtgen_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/evtgen/2.0.0-03d41481353453f82dd350686b161ba5/include
evtgen_EX_LIB := EvtGen EvtGenExternal
evtgen_EX_USE := hepmc pythia8 tauolapp photospp

ALL_TOOLS      += expat
expat_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/expat/2.4.8-b093687a482bf386f8f8c236c5b2efa2/include
expat_EX_LIB := expat
expat_EX_USE := root_cxxdefaults

ALL_TOOLS      += fastjet-contrib-archive
fastjet-contrib-archive_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/fastjet-contrib/1.051-afd609f10b5f81984bb8b0a86a7935ee/include
fastjet-contrib-archive_EX_LIB := EnergyCorrelator GenericSubtractor JetCleanser JetFFMoments Nsubjettiness ScJet SubjetCounting VariableR

ALL_TOOLS      += fastjet-contrib
fastjet-contrib_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/fastjet-contrib/1.051-afd609f10b5f81984bb8b0a86a7935ee/include
fastjet-contrib_EX_LIB := fastjetcontribfragile

ALL_TOOLS      += fastjet
fastjet_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/fastjet/3.4.1-cc6ce46a28fb676c9080adb13b99f03c/include
fastjet_EX_LIB := fastjetplugins fastjettools siscone siscone_spherical fastjet
fastjet_EX_USE := root_cxxdefaults

ALL_TOOLS      += fastjet_x86-64-v3

ALL_TOOLS      += fftjet
fftjet_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/fftjet/1.5.0-4c95f831c41530ec8fa31a87717d9e5b/include
fftjet_EX_LIB := fftjet
fftjet_EX_USE := root_cxxdefaults

ALL_TOOLS      += fftw3
fftw3_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/fftw3/3.3.8-5270bdf9998065b38f91e9aa053c73ec/include
fftw3_EX_LIB := fftw3
fftw3_EX_USE := root_cxxdefaults

ALL_TOOLS      += flatbuffers
flatbuffers_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/flatbuffers/2.0.6-2194dda9a65f37dd851c53993c66d44b/include
flatbuffers_EX_LIB := flatbuffers

ALL_TOOLS      += fmt
fmt_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/fmt/8.0.1-258b4791803c34b7e98cf43693e54d87/include
fmt_EX_LIB := fmt

ALL_TOOLS      += freetype
freetype_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/freetype/2.10.0-5451e8098ba14d8050964be923980663/include
freetype_EX_LIB := freetype-cms
freetype_EX_USE := root_cxxdefaults

ALL_TOOLS      += frontier_client
frontier_client_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/frontier_client/2.10.2-5398b7713cf60f9e7abeff19a234de65/include
frontier_client_EX_LIB := frontier_client
frontier_client_EX_USE := root_cxxdefaults zlib expat

ALL_TOOLS      += g4hepem_interface
g4hepem_interface_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/g4hepem/20230309-b0584df520fc175f0deaed3bce7356e6/include/G4HepEm

ALL_TOOLS      += g4hepemcore
g4hepemcore_EX_LIB := g4HepEmData g4HepEmInit g4HepEmRun g4HepEm
g4hepemcore_EX_USE := g4hepem_interface

ALL_TOOLS      += g4hepemstatic
g4hepemstatic_EX_LIB := g4hepem-static
g4hepemstatic_EX_USE := g4hepem_interface

ALL_TOOLS      += gbl
gbl_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/gbl/V03-01-01-4e8a2c89464fb6eb4977370503878bd7/include
gbl_EX_LIB := GBL
gbl_EX_USE := eigen

ALL_TOOLS      += gbl_x86-64-v3

ALL_TOOLS      += gcc-atomic
gcc-atomic_EX_LIB := atomic

ALL_TOOLS      += gcc-ccompiler
gcc-ccompiler_EX_FLAGS_CFLAGS  := -O2 -pthread
gcc-ccompiler_EX_FLAGS_CSHAREDOBJECTFLAGS  := -fPIC

ALL_TOOLS      += gcc-cxxcompiler
gcc-cxxcompiler_EX_FLAGS_CPPDEFINES  := -DGNU_GCC -D_GNU_SOURCE
gcc-cxxcompiler_EX_FLAGS_CXXFLAGS  := -O2 -pthread -pipe -Werror=main -Werror=pointer-arith -Werror=overlength-strings -Wno-vla -Werror=overflow -std=c++17 -ftree-vectorize -Werror=array-bounds -Werror=format-contains-nul -Werror=type-limits -fvisibility-inlines-hidden -fno-math-errno --param vect-max-version-for-alias-checks=50 -Xassembler --compress-debug-sections -Wno-error=array-bounds -Warray-bounds -fuse-ld=bfd -march=x86-64-v2 -felide-constructors -fmessage-length=0 -Wall -Wno-non-template-friend -Wno-long-long -Wreturn-type -Wextra -Wpessimizing-move -Wclass-memaccess -Wno-cast-function-type -Wno-unused-but-set-parameter -Wno-ignored-qualifiers -Wno-unused-parameter -Wunused -Wparentheses -Werror=return-type -Werror=missing-braces -Werror=unused-value -Werror=unused-label -Werror=address -Werror=format -Werror=sign-compare -Werror=write-strings -Werror=delete-non-virtual-dtor -Werror=strict-aliasing -Werror=narrowing -Werror=unused-but-set-variable -Werror=reorder -Werror=unused-variable -Werror=conversion-null -Werror=return-local-addr -Wnon-virtual-dtor -Werror=switch -fdiagnostics-show-option -Wno-unused-local-typedefs -Wno-attributes -Wno-psabi
gcc-cxxcompiler_EX_FLAGS_CXXFLAGS_TARGETS_X86_64_V3  := -march=x86-64-v3
gcc-cxxcompiler_EX_FLAGS_CXXSHAREDFLAGS  := -shared -Wl,-E
gcc-cxxcompiler_EX_FLAGS_CXXSHAREDOBJECTFLAGS  := -fPIC
gcc-cxxcompiler_EX_FLAGS_LDFLAGS  := -Wl,-E -Wl,--hash-style=gnu
gcc-cxxcompiler_EX_FLAGS_LD_UNIT  := -r -z muldefs
gcc-cxxcompiler_EX_FLAGS_LTO_FLAGS  := -flto=auto -fipa-icf -flto-odr-type-merging -fno-fat-lto-objects -Wodr
gcc-cxxcompiler_EX_FLAGS_PGO_FLAGS  := -fprofile-prefix-path=$(LOCALTOP) -fprofile-update=prefer-atomic -fprofile-correction
gcc-cxxcompiler_EX_FLAGS_PGO_GENERATE_FLAGS  := -fprofile-generate=%q{CMSSW_PGO_DIRECTORY}/cmssw/%q{CMSSW_CPU_TYPE}
gcc-cxxcompiler_EX_FLAGS_PGO_USE_FLAGS  := -fprofile-use=@{CMSSW_PGO_DIRECTORY}/cmssw/@{CMSSW_CPU_TYPE} -fprofile-partial-training
gcc-cxxcompiler_EX_FLAGS_REM_CXXFLAGS_TARGETS_X86_64_V3  := -march=x86-64-v2

ALL_TOOLS      += gcc-f77compiler
gcc-f77compiler_EX_LIB := gfortran m
gcc-f77compiler_EX_FLAGS_FFLAGS  := -fno-second-underscore -Wunused -Wuninitialized -O2 -cpp -std=legacy
gcc-f77compiler_EX_FLAGS_FOPTIMISEDFLAGS  := -O2
gcc-f77compiler_EX_FLAGS_FSHAREDOBJECTFLAGS  := -fPIC

ALL_TOOLS      += gcc-plugin
gcc-plugin_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/gcc/12.3.1-40d504be6370b5a30e3947a6e575ca28/bin/../lib/gcc/x86_64-redhat-linux-gnu/12.3.1/plugin/include
gcc-plugin_EX_LIB := cc1plugin cp1plugin

ALL_TOOLS      += gdb

ALL_TOOLS      += gdbm
gdbm_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/gdbm/1.10-1f0ec10a142f21150eef9d32ba579779/include
gdbm_EX_LIB := gdbm
gdbm_EX_USE := root_cxxdefaults

ALL_TOOLS      += gdrcopy
gdrcopy_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/gdrcopy/2.4.1-90805e5c64de9624058c459fad9b82db/include
gdrcopy_EX_LIB := gdrapi

ALL_TOOLS      += geant4
geant4_EX_USE := geant4core geant4vis

ALL_TOOLS      += geant4_interface
geant4_interface_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/geant4/11.2.1-49a8fc8238ba116d3207a3e0252e352f/include/Geant4 /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/geant4/11.2.1-49a8fc8238ba116d3207a3e0252e352f/include
geant4_interface_EX_USE := clhep vecgeom zlib expat xerces-c root_cxxdefaults
geant4_interface_EX_FLAGS_CPPDEFINES  := -DGNU_GCC -DG4V9
geant4_interface_EX_FLAGS_CXXFLAGS  := -ftls-model=global-dynamic -pthread

ALL_TOOLS      += geant4core
geant4core_EX_LIB := G4digits_hits G4error_propagation G4event G4geometry G4global G4graphics_reps G4intercoms G4interfaces G4materials G4parmodels G4particles G4geomtext G4mctruth G4gdml G4physicslists G4processes G4readout G4run G4tracking G4track G4analysis
geant4core_EX_USE := geant4_interface
geant4core_EX_FLAGS_CPPDEFINES  := -DGNU_GCC -DG4V9

ALL_TOOLS      += geant4data

ALL_TOOLS      += geant4static
geant4static_EX_LIB := geant4-static
geant4static_EX_USE := geant4_interface

ALL_TOOLS      += geant4vis
geant4vis_EX_LIB := G4FR G4modeling G4RayTracer G4Tree G4visHepRep G4vis_management G4VRML G4GMocren
geant4vis_EX_USE := geant4core

ALL_TOOLS      += giflib
giflib_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/giflib/5.2.0-7d08baa8cb7eb0f890fa0e6ac003a997/include
giflib_EX_LIB := gif
giflib_EX_USE := root_cxxdefaults

ALL_TOOLS      += git

ALL_TOOLS      += glimpse

ALL_TOOLS      += gmake

ALL_TOOLS      += gnuplot

ALL_TOOLS      += google-benchmark-main
google-benchmark-main_EX_LIB := benchmark_main
google-benchmark-main_EX_USE := google-benchmark

ALL_TOOLS      += google-benchmark
google-benchmark_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/google-benchmark/1.7.x-6aab12faac50c62f403eccdfe3a85312/include
google-benchmark_EX_LIB := benchmark
google-benchmark_EX_USE := sockets

ALL_TOOLS      += gosam

ALL_TOOLS      += gosamcontrib
gosamcontrib_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/gosamcontrib/2.0-20150803-3fd28e0213ceb2e9dc1eae1945774558/include

ALL_TOOLS      += gperf
gperf_EX_LIB := profiler
gperf_EX_USE := libunwind

ALL_TOOLS      += grpc
grpc_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/grpc/1.35.0-1ab211c07f3a93ccaa028715bcd16443/include
grpc_EX_LIB := grpc grpc++ grpc++_reflection
grpc_EX_USE := protobuf openssl pcre abseil-cpp c-ares re2
grpc_EX_FLAGS_SYSTEM_INCLUDE  := 1

ALL_TOOLS      += gsl
gsl_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/gsl/2.6-5e2ce72ea2977ff21a2344bbb52daf5c/include
gsl_EX_LIB := gsl
gsl_EX_USE := openblas root_cxxdefaults

ALL_TOOLS      += hdf5
hdf5_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/hdf5/1.14.1-d23b3b55a13ed922b5a8344b0b323f19/include
hdf5_EX_LIB := hdf5 hdf5_hl
hdf5_EX_USE := openmpi

ALL_TOOLS      += heaptrack

ALL_TOOLS      += hector
hector_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/hector/1.3.4_patch1-33b5833eb743a6a0a807ad5fb5b2cf69/include
hector_EX_LIB := Hector
hector_EX_USE := root_cxxdefaults

ALL_TOOLS      += hepmc
hepmc_EX_LIB := HepMCfio HepMC
hepmc_EX_USE := hepmc_headers

ALL_TOOLS      += hepmc3
hepmc3_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/hepmc3/3.2.7-eb6cff961e18f5c7426af506351f9026/include
hepmc3_EX_LIB := HepMC3 HepMC3search

ALL_TOOLS      += hepmc_headers
hepmc_headers_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/hepmc/2.06.10-cb64022df5945e7e8948ec2000290943/include
hepmc_headers_EX_USE := root_cxxdefaults

ALL_TOOLS      += heppdt
heppdt_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/heppdt/3.04.01-ccc20831a6ffcd82111849ab746d0527/include
heppdt_EX_LIB := HepPDT HepPID
heppdt_EX_USE := root_cxxdefaults

ALL_TOOLS      += herwig7
herwig7_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/herwig7/7.2.2-4efa7fc43561925ce2fd177c5b8841bf/include
herwig7_EX_LIB := HerwigAPI
herwig7_EX_USE := root_cxxdefaults lhapdf thepeg madgraph5amcatnlo openloops

ALL_TOOLS      += highfive
highfive_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/highfive/2.3.1-a073ebeba6610baa8c69b7d8a69fd224/include
highfive_EX_USE := boost hdf5

ALL_TOOLS      += histfactory
histfactory_EX_LIB := HistFactory
histfactory_EX_USE := roofitcore roofit roostats rootcore roothistmatrix rootgpad rootxml rootfoam

ALL_TOOLS      += hls
hls_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/hls/2019.08-0e37f055a3ed22611ce5edecb14d0695/include
hls_EX_USE := root_cxxdefaults
hls_EX_FLAGS_SYSTEM_INCLUDE  := 1

ALL_TOOLS      += hls4mlemulatorextras
hls4mlemulatorextras_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/hls4mlEmulatorExtras/1.1.3-7abf6dadd3e09d4bf89fb5c47d56db40/include
hls4mlemulatorextras_EX_LIB := emulator_interface
hls4mlemulatorextras_EX_USE := sockets

ALL_TOOLS      += hwloc
hwloc_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/hwloc/2.10.0-d3bc70064c3880967aa758085c370186/include/hwloc
hwloc_EX_LIB := hwloc

ALL_TOOLS      += hydjet
hydjet_EX_LIB := hydjet
hydjet_EX_USE := pyquen pythia6 lhapdf

ALL_TOOLS      += hydjet2
hydjet2_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/hydjet2/2.4.3-2c091c8344cdc1cd756115fbaf7fe577/include
hydjet2_EX_LIB := hydjet2
hydjet2_EX_USE := pyquen pythia6 lhapdf root
hydjet2_EX_FLAGS_SYSTEM_INCLUDE  := 1

ALL_TOOLS      += igprof

ALL_TOOLS      += intel-license

ALL_TOOLS      += ittnotify
ittnotify_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/ittnotify/16.06.18-e7bdf646a1ce643ba6ad35661112659a/include
ittnotify_EX_LIB := ittnotify

ALL_TOOLS      += iwyu-cxxcompiler
iwyu-cxxcompiler_EX_USE := llvm-cxxcompiler

ALL_TOOLS      += jemalloc-debug
jemalloc-debug_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/jemalloc-debug/5.3.0-236d4ae0556ad9cc58776d9e0b938bd5/include
jemalloc-debug_EX_LIB := jemalloc-debug
jemalloc-debug_EX_USE := root_cxxdefaults

ALL_TOOLS      += jemalloc-prof
jemalloc-prof_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/jemalloc-prof/5.3.0-aec7e6c183d39439d234563117a39491/include
jemalloc-prof_EX_LIB := jemalloc-prof
jemalloc-prof_EX_USE := root_cxxdefaults

ALL_TOOLS      += jemalloc
jemalloc_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/jemalloc/5.3.0-678a782b0537aa5ad6fa9f4dd9779f4d/include
jemalloc_EX_LIB := jemalloc
jemalloc_EX_USE := root_cxxdefaults

ALL_TOOLS      += json
json_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/json/3.11.3-b46e2b2561069b3e02d0f04ed9933293/include

ALL_TOOLS      += ktjet
ktjet_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/ktjet/1.06-c963d5a26e0755b608cff98b1b8eef76/include
ktjet_EX_LIB := KtEvent
ktjet_EX_USE := root_cxxdefaults
ktjet_EX_FLAGS_CPPDEFINES  := -DKTDOUBLEPRECISION

ALL_TOOLS      += l1metml

ALL_TOOLS      += lcov

ALL_TOOLS      += lhapdf
lhapdf_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/lhapdf/6.4.0-4aa384870ea6380250957dc033c78953/include
lhapdf_EX_LIB := LHAPDF
lhapdf_EX_USE := pythia6 root_cxxdefaults

ALL_TOOLS      += libffi
libffi_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/libffi/3.4.2-9aa8487f9a3ce11ee7e4e6433e82c48d/include
libffi_EX_LIB := ffi

ALL_TOOLS      += libibumad
libibumad_EX_LIB := ibumad
libibumad_EX_USE := rdma-core

ALL_TOOLS      += libibverbs
libibverbs_EX_LIB := ibverbs
libibverbs_EX_USE := rdma-core

ALL_TOOLS      += libjpeg-turbo
libjpeg-turbo_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/libjpeg-turbo/2.0.2-abd1969508d15f2a08cd087d6497118a/include
libjpeg-turbo_EX_LIB := jpeg turbojpeg
libjpeg-turbo_EX_USE := root_cxxdefaults

ALL_TOOLS      += libpciaccess
libpciaccess_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/libpciaccess/0.16-7bc5e8d81bd9ed21aa0bba50ac5f4a8a/include
libpciaccess_EX_LIB := libpciaccess

ALL_TOOLS      += libpng
libpng_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/libpng/1.6.37-83b21f0cf90ea53017db2db50b3e5a69/include
libpng_EX_LIB := png
libpng_EX_USE := root_cxxdefaults zlib

ALL_TOOLS      += librdmacm
librdmacm_EX_LIB := rdmacm
librdmacm_EX_USE := rdma-core libibverbs

ALL_TOOLS      += libtiff
libtiff_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/libtiff/4.0.10-cf3af61a37ed01e7d8f11625e8e31aad/include
libtiff_EX_LIB := tiff
libtiff_EX_USE := root_cxxdefaults libjpeg-turbo zlib

ALL_TOOLS      += libungif
libungif_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/libungif/4.1.4-2f91a77bad1564f8547f5d13089fd857/include
libungif_EX_LIB := ungif
libungif_EX_USE := root_cxxdefaults zlib

ALL_TOOLS      += libunwind
libunwind_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/libunwind/1.7.2-master-a068bbd2663c38f07d2f2e6cf84950b0/include
libunwind_EX_LIB := unwind
libunwind_EX_USE := root_cxxdefaults

ALL_TOOLS      += libuuid
libuuid_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/libuuid/2.34-27ce4c3579b5b1de2808ea9c4cd8ed29/include
libuuid_EX_LIB := uuid
libuuid_EX_USE := root_cxxdefaults sockets

ALL_TOOLS      += libxml2
libxml2_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/libxml2/2.9.10-e94a08e9a964ba6f9f66c3ebca88f254/include/libxml2
libxml2_EX_LIB := xml2
libxml2_EX_USE := root_cxxdefaults

ALL_TOOLS      += libxslt
libxslt_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/libxslt/1.1.28-32341fe98e89682dff4e107aab98d0af/include/libxslt
libxslt_EX_LIB := xslt

ALL_TOOLS      += libzmq
libzmq_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/libzmq/4.3.5-5bf04ee-e25c7ba201185c1256c3f6247b315b91/include
libzmq_EX_LIB := zmq

ALL_TOOLS      += llvm-analyzer-ccompiler
llvm-analyzer-ccompiler_EX_USE := llvm-ccompiler

ALL_TOOLS      += llvm-analyzer-cxxcompiler
llvm-analyzer-cxxcompiler_EX_USE := llvm-cxxcompiler

ALL_TOOLS      += llvm-ccompiler
llvm-ccompiler_EX_USE := gcc-ccompiler
llvm-ccompiler_EX_FLAGS_CFLAGS  := --gcc-toolchain=/cvmfs/cms.cern.ch/el9_amd64_gcc12/external/gcc/12.3.1-40d504be6370b5a30e3947a6e575ca28

ALL_TOOLS      += llvm-cxxcompiler
llvm-cxxcompiler_EX_USE := gcc-cxxcompiler
llvm-cxxcompiler_EX_FLAGS_CXXFLAGS  := -Wno-c99-extensions -Wno-c++11-narrowing -D__STRICT_ANSI__ -Wno-unused-private-field -Wno-unknown-pragmas -Wno-unused-command-line-argument -Wno-unknown-warning-option -ftemplate-depth=512 -Wno-error=potentially-evaluated-expression -Wno-tautological-type-limit-compare -fsized-deallocation --gcc-toolchain=/cvmfs/cms.cern.ch/el9_amd64_gcc12/external/gcc/12.3.1-40d504be6370b5a30e3947a6e575ca28
llvm-cxxcompiler_EX_FLAGS_REM_CXXFLAGS  := -Wno-non-template-friend -Werror=format-contains-nul -Werror=maybe-uninitialized -Werror=unused-but-set-variable -Werror=return-local-addr -fipa-pta -frounding-math -mrecip -fno-crossjumping -fno-aggressive-loop-optimizations -funroll-all-loops
llvm-cxxcompiler_EX_FLAGS_REM_LTO_FLAGS  := -fipa-icf -flto-odr-type-merging -fno-fat-lto-objects
llvm-cxxcompiler_EX_FLAGS_REM_PGO_FLAGS  := -fprofile%

ALL_TOOLS      += llvm-f77compiler
llvm-f77compiler_EX_USE := gcc-f77compiler

ALL_TOOLS      += llvm
llvm_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/llvm/17.0.3-c63837ad0d3b09642e66207455f39fed/include
llvm_EX_LIB := clang
llvm_EX_FLAGS_CXXFLAGS  := -D_DEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -O3 -fomit-frame-pointer -fPIC -Wno-enum-compare -Wno-strict-aliasing -fno-rtti
llvm_EX_FLAGS_LDFLAGS  := -Wl,-undefined -Wl,suppress

ALL_TOOLS      += log4cplus
log4cplus_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/log4cplus/2.0.7-ae8787314b6d5643b9c498894820e8c9/include
log4cplus_EX_LIB := log4cplusS

ALL_TOOLS      += lwtnn
lwtnn_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/lwtnn/2.14.1-2c0930f0f807b3dbe81a1ea7655dbd0d/include
lwtnn_EX_LIB := lwtnn
lwtnn_EX_USE := root_cxxdefaults eigen boost_system

ALL_TOOLS      += lwtnn_x86-64-v3

ALL_TOOLS      += lz4
lz4_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/lz4/1.9.2-e478fcd3e5e191d5bb4ade190474ad76/include
lz4_EX_LIB := lz4
lz4_EX_USE := root_cxxdefaults

ALL_TOOLS      += madgraph5amcatnlo
madgraph5amcatnlo_EX_USE := root_cxxdefaults gosamcontrib

ALL_TOOLS      += mctester
mctester_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/mctester/1.25.1-586370686498cb537c6afdefb167dd74/include
mctester_EX_LIB := HEPEvent HepMCEvent MCTester
mctester_EX_USE := root_cxxdefaults root hepmc

ALL_TOOLS      += md5
md5_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/md5/1.0.0-5b594b264e04ae51e893b1d69a797ec6/include
md5_EX_LIB := cms-md5

ALL_TOOLS      += meschach
meschach_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/meschach/1.2.pCMS1-79435924678a8cc522f783c34e3865d7/include
meschach_EX_LIB := meschach
meschach_EX_USE := root_cxxdefaults

ALL_TOOLS      += millepede
millepede_EX_USE := sockets pcre zlib

ALL_TOOLS      += mozsearch
mozsearch_EX_FLAGS_INDEX_FLAGS  := -Xclang -load -Xclang libclang-index-plugin.so -Xclang -add-plugin -Xclang mozsearch-index -Xclang -plugin-arg-mozsearch-index -Xclang src/ -Xclang -plugin-arg-mozsearch-index -Xclang mozsearch/ -Xclang -plugin-arg-mozsearch-index -Xclang tmp/el9_amd64_gcc12/src/

ALL_TOOLS      += numactl
numactl_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/numactl/2.0.14-b52604abfe0049e6a7aca40a3d26c569/include
numactl_EX_LIB := numa

ALL_TOOLS      += numpy-c-api
numpy-c-api_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/py3-numpy/1.24.3-ac08ea497df571aa0b37dc29bb7a2045/c-api/core/include
numpy-c-api_EX_LIB := npymath
numpy-c-api_EX_USE := root_cxxdefaults

ALL_TOOLS      += nvidia-drivers
nvidia-drivers_EX_LIBDIR := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/cuda/12.4.1-8b491273511f117bf853a3f82dc2d34f/drivers
nvidia-drivers_EX_FLAGS_SKIP_TOOL_SYMLINKS  := 1

ALL_TOOLS      += nvperf
nvperf_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/cuda/12.4.1-8b491273511f117bf853a3f82dc2d34f/include
nvperf_EX_LIB := nvperf_host nvperf_target

ALL_TOOLS      += ofast-flag
ofast-flag_EX_FLAGS_CXXFLAGS  := -Ofast -fno-reciprocal-math -mrecip=none
ofast-flag_EX_FLAGS_NO_RECURSIVE_EXPORT  := 1

ALL_TOOLS      += onnxruntime
onnxruntime_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/onnxruntime/1.17.1-751978a407237e8a75dd923166e227c3/include
onnxruntime_EX_LIB := onnxruntime
onnxruntime_EX_USE := protobuf cuda cudnn re2

ALL_TOOLS      += openblas
openblas_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/OpenBLAS/0.3.15-c877ab57fa7b04ce290093588c6c5717/include
openblas_EX_LIB := openblas

ALL_TOOLS      += openblas_x86-64-v3

ALL_TOOLS      += opencv
opencv_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/opencv/4.9.0-6924bd12d27ed41e7748d2bf486dce15/include
opencv_EX_LIB := opencv_core
opencv_EX_USE := libpng libjpeg-turbo zlib eigen openblas

ALL_TOOLS      += opencv_x86-64-v3

ALL_TOOLS      += opengl
opengl_EX_LIB := GL GLU
opengl_EX_USE := x11

ALL_TOOLS      += openldap
openldap_EX_USE := db6

ALL_TOOLS      += openloops

ALL_TOOLS      += openmpi
openmpi_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/openmpi/4.1.6-b710c18be379142e0d5442f5ec62a712/include
openmpi_EX_LIB := mpi mpi_cxx

ALL_TOOLS      += openssl
openssl_EX_LIB := ssl crypto

ALL_TOOLS      += oracle
oracle_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/oracle/19.11.0.0.0dbru-0ef9ee763c1e7a90d8c4515a5af97f0b/include
oracle_EX_LIB := clntsh
oracle_EX_USE := root_cxxdefaults sockets

ALL_TOOLS      += oracleocci
oracleocci_EX_LIB := occi
oracleocci_EX_USE := oracle

ALL_TOOLS      += pacparser
pacparser_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pacparser/1.4.2-d885ac8c81efb68d64e5a3e0df79c00e/include
pacparser_EX_LIB := pacparser
pacparser_EX_USE := root_cxxdefaults

ALL_TOOLS      += pcre
pcre_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pcre/8.43-e34796d17981e9b6d174328c69446455/include
pcre_EX_LIB := pcre
pcre_EX_USE := root_cxxdefaults zlib bz2lib

ALL_TOOLS      += pcre2
pcre2_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pcre2/10.36-c75e887aa283481338a9a2e3cf0b953b/include
pcre2_EX_LIB := pcre2
pcre2_EX_USE := root_cxxdefaults zlib bz2lib

ALL_TOOLS      += photospp
photospp_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/photospp/3.64-e0708c0bb40dc037ba44b1bf7c008433/include
photospp_EX_LIB := Photospp PhotosppHepMC PhotosppHEPEVT
photospp_EX_USE := hepmc

ALL_TOOLS      += professor2
professor2_EX_USE := py3-numpy root yoda eigen

ALL_TOOLS      += protobuf
protobuf_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/protobuf/3.21.9-9f4f88a01a26339d3f9c9fcc0ee8a6d1/include
protobuf_EX_LIB := protobuf
protobuf_EX_USE := root_cxxdefaults

ALL_TOOLS      += py3-absl-py

ALL_TOOLS      += py3-annotated-types

ALL_TOOLS      += py3-anyio

ALL_TOOLS      += py3-appdirs

ALL_TOOLS      += py3-argon2-cffi-bindings

ALL_TOOLS      += py3-argon2-cffi

ALL_TOOLS      += py3-argparse

ALL_TOOLS      += py3-asn1crypto

ALL_TOOLS      += py3-astor

ALL_TOOLS      += py3-astroid

ALL_TOOLS      += py3-asttokens

ALL_TOOLS      += py3-astunparse

ALL_TOOLS      += py3-async-lru

ALL_TOOLS      += py3-atomicwrites

ALL_TOOLS      += py3-attrs

ALL_TOOLS      += py3-autopep8

ALL_TOOLS      += py3-avro

ALL_TOOLS      += py3-awkward-cpp

ALL_TOOLS      += py3-awkward-pandas

ALL_TOOLS      += py3-awkward

ALL_TOOLS      += py3-babel

ALL_TOOLS      += py3-backcall

ALL_TOOLS      += py3-backports-entry-points-selectable

ALL_TOOLS      += py3-backports-tarfile

ALL_TOOLS      += py3-beautifulsoup4

ALL_TOOLS      += py3-beniget

ALL_TOOLS      += py3-bleach

ALL_TOOLS      += py3-blosc2

ALL_TOOLS      += py3-boost-histogram

ALL_TOOLS      += py3-bottleneck

ALL_TOOLS      += py3-cachecontrol

ALL_TOOLS      += py3-cachetools

ALL_TOOLS      += py3-cachy

ALL_TOOLS      += py3-calver

ALL_TOOLS      += py3-certifi

ALL_TOOLS      += py3-cffi

ALL_TOOLS      += py3-chardet

ALL_TOOLS      += py3-charset-normalizer

ALL_TOOLS      += py3-cleo

ALL_TOOLS      += py3-click

ALL_TOOLS      += py3-clikit

ALL_TOOLS      += py3-cloudpickle

ALL_TOOLS      += py3-cms-tfaot

ALL_TOOLS      += py3-cmsml

ALL_TOOLS      += py3-colorama

ALL_TOOLS      += py3-comm

ALL_TOOLS      += py3-commonmark

ALL_TOOLS      += py3-contextlib2

ALL_TOOLS      += py3-contourpy

ALL_TOOLS      += py3-correctionlib

ALL_TOOLS      += py3-coverage

ALL_TOOLS      += py3-cppy

ALL_TOOLS      += py3-cramjam

ALL_TOOLS      += py3-crashtest

ALL_TOOLS      += py3-cryptography

ALL_TOOLS      += py3-cx-oracle

ALL_TOOLS      += py3-cycler

ALL_TOOLS      += py3-cython

ALL_TOOLS      += py3-dask-awkward

ALL_TOOLS      += py3-dask

ALL_TOOLS      += py3-debugpy

ALL_TOOLS      += py3-decorator

ALL_TOOLS      += py3-defusedxml

ALL_TOOLS      += py3-deprecated

ALL_TOOLS      += py3-deprecation

ALL_TOOLS      += py3-dill

ALL_TOOLS      += py3-distlib

ALL_TOOLS      += py3-distro

ALL_TOOLS      += py3-docopt

ALL_TOOLS      += py3-docutils

ALL_TOOLS      += py3-dulwich

ALL_TOOLS      += py3-dxr

ALL_TOOLS      += py3-editables

ALL_TOOLS      += py3-entrypoints

ALL_TOOLS      += py3-exceptiongroup

ALL_TOOLS      += py3-execnet

ALL_TOOLS      += py3-executing

ALL_TOOLS      += py3-fastjsonschema

ALL_TOOLS      += py3-filelock

ALL_TOOLS      += py3-fire

ALL_TOOLS      += py3-flake8

ALL_TOOLS      += py3-flatbuffers

ALL_TOOLS      += py3-flawfinder

ALL_TOOLS      += py3-flit-core

ALL_TOOLS      += py3-flit

ALL_TOOLS      += py3-fonttools

ALL_TOOLS      += py3-fsspec

ALL_TOOLS      += py3-funcsigs

ALL_TOOLS      += py3-future

ALL_TOOLS      += py3-gast

ALL_TOOLS      += py3-gitdb

ALL_TOOLS      += py3-gitpython

ALL_TOOLS      += py3-google-auth-oauthlib

ALL_TOOLS      += py3-google-auth

ALL_TOOLS      += py3-google-pasta

ALL_TOOLS      += py3-grpcio-tools

ALL_TOOLS      += py3-grpcio

ALL_TOOLS      += py3-h11

ALL_TOOLS      += py3-h5py

ALL_TOOLS      += py3-hatch-fancy-pypi-readme

ALL_TOOLS      += py3-hatch-vcs

ALL_TOOLS      += py3-hatchling

ALL_TOOLS      += py3-hepdata-lib

ALL_TOOLS      += py3-hepdata-validator

ALL_TOOLS      += py3-hist

ALL_TOOLS      += py3-histbook

ALL_TOOLS      += py3-histogrammar

ALL_TOOLS      += py3-histoprint

ALL_TOOLS      += py3-html5lib

ALL_TOOLS      += py3-httpcore

ALL_TOOLS      += py3-httpx

ALL_TOOLS      += py3-idna

ALL_TOOLS      += py3-importlib-metadata

ALL_TOOLS      += py3-importlib-resources

ALL_TOOLS      += py3-iniconfig

ALL_TOOLS      += py3-ipaddress

ALL_TOOLS      += py3-ipykernel

ALL_TOOLS      += py3-ipython

ALL_TOOLS      += py3-ipython_genutils

ALL_TOOLS      += py3-ipywidgets

ALL_TOOLS      += py3-isort

ALL_TOOLS      += py3-jaraco-classes

ALL_TOOLS      += py3-jaraco-context

ALL_TOOLS      += py3-jaraco-functools

ALL_TOOLS      += py3-jax

ALL_TOOLS      += py3-jedi

ALL_TOOLS      += py3-jeepney

ALL_TOOLS      += py3-jinja2

ALL_TOOLS      += py3-joblib

ALL_TOOLS      += py3-json5

ALL_TOOLS      += py3-jsonpickle

ALL_TOOLS      += py3-jsonschema-specifications

ALL_TOOLS      += py3-jsonschema

ALL_TOOLS      += py3-jupyter-client

ALL_TOOLS      += py3-jupyter-console

ALL_TOOLS      += py3-jupyter-core

ALL_TOOLS      += py3-jupyter-events

ALL_TOOLS      += py3-jupyter-lsp

ALL_TOOLS      += py3-jupyter-packaging

ALL_TOOLS      += py3-jupyter-server-terminals

ALL_TOOLS      += py3-jupyter-server

ALL_TOOLS      += py3-jupyterlab-pygments

ALL_TOOLS      += py3-jupyterlab-server

ALL_TOOLS      += py3-jupyterlab-widgets

ALL_TOOLS      += py3-jupyterlab

ALL_TOOLS      += py3-keras-applications

ALL_TOOLS      += py3-keras-preprocessing

ALL_TOOLS      += py3-keras

ALL_TOOLS      += py3-keras2onnx

ALL_TOOLS      += py3-keyring

ALL_TOOLS      += py3-kiwisolver

ALL_TOOLS      += py3-law

ALL_TOOLS      += py3-lazy-object-proxy

ALL_TOOLS      += py3-lizard

ALL_TOOLS      += py3-locket

ALL_TOOLS      += py3-lockfile

ALL_TOOLS      += py3-luigi

ALL_TOOLS      += py3-lxml

ALL_TOOLS      += py3-lz4

ALL_TOOLS      += py3-mako

ALL_TOOLS      += py3-markdown-it-py

ALL_TOOLS      += py3-markdown

ALL_TOOLS      += py3-markupsafe

ALL_TOOLS      += py3-matplotlib-inline

ALL_TOOLS      += py3-matplotlib

ALL_TOOLS      += py3-mccabe

ALL_TOOLS      += py3-mdurl

ALL_TOOLS      += py3-meson-python

ALL_TOOLS      += py3-meson

ALL_TOOLS      += py3-mistune

ALL_TOOLS      += py3-ml_dtypes

ALL_TOOLS      += py3-mock

ALL_TOOLS      += py3-more-itertools

ALL_TOOLS      += py3-mplhep-data

ALL_TOOLS      += py3-mplhep

ALL_TOOLS      += py3-mpmath

ALL_TOOLS      += py3-msgpack

ALL_TOOLS      += py3-nbclient

ALL_TOOLS      += py3-nbconvert

ALL_TOOLS      += py3-nbformat

ALL_TOOLS      += py3-ndindex

ALL_TOOLS      += py3-nest-asyncio

ALL_TOOLS      += py3-networkx

ALL_TOOLS      += py3-notebook-shim

ALL_TOOLS      += py3-notebook

ALL_TOOLS      += py3-numexpr

ALL_TOOLS      += py3-numpy

ALL_TOOLS      += py3-nvidia-ml-py

ALL_TOOLS      += py3-oauthlib

ALL_TOOLS      += py3-onnx

ALL_TOOLS      += py3-onnxconverter-common

ALL_TOOLS      += py3-onnxmltools

ALL_TOOLS      += py3-opt-einsum

ALL_TOOLS      += py3-packaging

ALL_TOOLS      += py3-pandas

ALL_TOOLS      += py3-pandocfilters

ALL_TOOLS      += py3-parsimonious

ALL_TOOLS      += py3-parso

ALL_TOOLS      += py3-partd

ALL_TOOLS      += py3-pastel

ALL_TOOLS      += py3-pathlib2

ALL_TOOLS      += py3-pathspec

ALL_TOOLS      += py3-pbr

ALL_TOOLS      += py3-pexpect

ALL_TOOLS      += py3-pickleshare

ALL_TOOLS      += py3-pillow

ALL_TOOLS      += py3-pip

ALL_TOOLS      += py3-pkgconfig

ALL_TOOLS      += py3-pkginfo

ALL_TOOLS      += py3-plac

ALL_TOOLS      += py3-platformdirs

ALL_TOOLS      += py3-pluggy

ALL_TOOLS      += py3-ply

ALL_TOOLS      += py3-poetry-core

ALL_TOOLS      += py3-poetry-plugin-export

ALL_TOOLS      += py3-poetry

ALL_TOOLS      += py3-prettytable

ALL_TOOLS      += py3-prometheus-client

ALL_TOOLS      += py3-prompt_toolkit

ALL_TOOLS      += py3-protobuf

ALL_TOOLS      += py3-prwlock

ALL_TOOLS      += py3-psutil

ALL_TOOLS      += py3-ptyprocess

ALL_TOOLS      += py3-pure-eval

ALL_TOOLS      += py3-py-cpuinfo

ALL_TOOLS      += py3-pyasn1-modules

ALL_TOOLS      += py3-pyasn1

ALL_TOOLS      += py3-pybind11
py3-pybind11_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/py3-pybind11/2.12.0-5fdc582b2132b51b59541f7f11c0e2cb/lib/python3.9/site-packages/pybind11/include

ALL_TOOLS      += py3-pycodestyle

ALL_TOOLS      += py3-pycparser

ALL_TOOLS      += py3-pycuda

ALL_TOOLS      += py3-pycurl

ALL_TOOLS      += py3-pydantic-core

ALL_TOOLS      += py3-pydantic

ALL_TOOLS      += py3-pydot

ALL_TOOLS      += py3-pyflakes

ALL_TOOLS      += py3-pygithub

ALL_TOOLS      += py3-pygments

ALL_TOOLS      += py3-pyjwt

ALL_TOOLS      += py3-pylev

ALL_TOOLS      += py3-pylint

ALL_TOOLS      += py3-pynacl

ALL_TOOLS      += py3-pyparsing

ALL_TOOLS      += py3-pyproject-metadata

ALL_TOOLS      += py3-pysocks

ALL_TOOLS      += py3-pysqlite3

ALL_TOOLS      += py3-pytest-cov

ALL_TOOLS      += py3-pytest-runner

ALL_TOOLS      += py3-pytest-xdist

ALL_TOOLS      += py3-pytest

ALL_TOOLS      += py3-python-daemon

ALL_TOOLS      += py3-python-dateutil

ALL_TOOLS      += py3-python-json-logger

ALL_TOOLS      += py3-python-ldap

ALL_TOOLS      += py3-python-rapidjson

ALL_TOOLS      += py3-pythran

ALL_TOOLS      += py3-pytoml

ALL_TOOLS      += py3-pytools

ALL_TOOLS      += py3-pytz

ALL_TOOLS      += py3-pyyaml

ALL_TOOLS      += py3-pyzmq

ALL_TOOLS      += py3-rapidfuzz

ALL_TOOLS      += py3-referencing

ALL_TOOLS      += py3-regex

ALL_TOOLS      += py3-repoze-lru

ALL_TOOLS      += py3-requests-oauthlib

ALL_TOOLS      += py3-requests-toolbelt

ALL_TOOLS      += py3-requests

ALL_TOOLS      += py3-rfc3339-validator

ALL_TOOLS      += py3-rfc3986-validator

ALL_TOOLS      += py3-rich

ALL_TOOLS      += py3-rpds-py

ALL_TOOLS      += py3-rsa

ALL_TOOLS      += py3-scandir

ALL_TOOLS      += py3-schema

ALL_TOOLS      += py3-scikit-build

ALL_TOOLS      += py3-scikit-learn

ALL_TOOLS      += py3-scinum

ALL_TOOLS      += py3-scipy

ALL_TOOLS      += py3-seaborn

ALL_TOOLS      += py3-secretstorage

ALL_TOOLS      += py3-semantic-version

ALL_TOOLS      += py3-send2trash

ALL_TOOLS      += py3-setuptools-rust

ALL_TOOLS      += py3-setuptools-scm

ALL_TOOLS      += py3-setuptools

ALL_TOOLS      += py3-shellingham

ALL_TOOLS      += py3-simplegeneric

ALL_TOOLS      += py3-singledispatch

ALL_TOOLS      += py3-six

ALL_TOOLS      += py3-skl2onnx

ALL_TOOLS      += py3-smmap

ALL_TOOLS      += py3-sniffio

ALL_TOOLS      += py3-soupsieve

ALL_TOOLS      += py3-sqlalchemy

ALL_TOOLS      += py3-stack-data

ALL_TOOLS      += py3-stevedore

ALL_TOOLS      += py3-subprocess32

ALL_TOOLS      += py3-sympy

ALL_TOOLS      += py3-tables

ALL_TOOLS      += py3-tabulate

ALL_TOOLS      += py3-tenacity

ALL_TOOLS      += py3-tensorboard-data-server

ALL_TOOLS      += py3-tensorboard-plugin-wit

ALL_TOOLS      += py3-tensorboard

ALL_TOOLS      += py3-tensorflow-estimator

ALL_TOOLS      += py3-tensorflow-io-gcs-filesystem

ALL_TOOLS      += py3-tensorflow

ALL_TOOLS      += py3-termcolor

ALL_TOOLS      += py3-terminado

ALL_TOOLS      += py3-testpath

ALL_TOOLS      += py3-threadpoolctl

ALL_TOOLS      += py3-tinycss2

ALL_TOOLS      += py3-toml

ALL_TOOLS      += py3-tomli-w

ALL_TOOLS      += py3-tomli

ALL_TOOLS      += py3-tomlkit

ALL_TOOLS      += py3-toolz

ALL_TOOLS      += py3-tornado

ALL_TOOLS      += py3-tqdm

ALL_TOOLS      += py3-traitlets

ALL_TOOLS      += py3-trove-classifiers

ALL_TOOLS      += py3-typed-ast

ALL_TOOLS      += py3-typing-extensions

ALL_TOOLS      += py3-tzdata

ALL_TOOLS      += py3-uhi

ALL_TOOLS      += py3-uncertainties

ALL_TOOLS      += py3-uproot

ALL_TOOLS      += py3-urllib3

ALL_TOOLS      += py3-vector

ALL_TOOLS      += py3-versioneer

ALL_TOOLS      += py3-virtualenv-clone

ALL_TOOLS      += py3-virtualenv

ALL_TOOLS      += py3-virtualenvwrapper

ALL_TOOLS      += py3-wcwidth

ALL_TOOLS      += py3-webencodings

ALL_TOOLS      += py3-websocket-client

ALL_TOOLS      += py3-werkzeug

ALL_TOOLS      += py3-wheel

ALL_TOOLS      += py3-widgetsnbextension

ALL_TOOLS      += py3-wrapt

ALL_TOOLS      += py3-xgboost

ALL_TOOLS      += py3-zipp

ALL_TOOLS      += pyclang

ALL_TOOLS      += pydata
pydata_EX_FLAGS_LDFLAGS  := $(PYDATA_BASE)/lib/pydata.o
pydata_EX_FLAGS_NO_RECURSIVE_EXPORT  := 1

ALL_TOOLS      += pyquen
pyquen_EX_LIB := pyquen
pyquen_EX_USE := pythia6 lhapdf

ALL_TOOLS      += pythia6
pythia6_EX_LIB := pythia6 pythia6_dummy pythia6_pdfdummy
pythia6_EX_USE := pythia6_headers f77compiler

ALL_TOOLS      += pythia6_headers
pythia6_headers_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pythia6/426-c430ea6f2967f65248af15c71e6c653e/include
pythia6_headers_EX_USE := root_cxxdefaults

ALL_TOOLS      += pythia8
pythia8_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pythia8/311-d54d91d562ffbdcc79072e3aa3897e0b/include
pythia8_EX_LIB := pythia8
pythia8_EX_USE := root_cxxdefaults cxxcompiler hepmc3 hepmc lhapdf
pythia8_EX_FLAGS_SYSTEM_INCLUDE  := 1

ALL_TOOLS      += python-paths

ALL_TOOLS      += python3
python3_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/python3/3.9.14-b98669a1dd7ad6f9a9402f3cf1f9d11c/include/python3.9
python3_EX_LIB := python3.9
python3_EX_USE := sockets

ALL_TOOLS      += python_tools

ALL_TOOLS      += pytorch-cluster
pytorch-cluster_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pytorch-cluster/1.6.3-d29ee1aa75a636f15d60189c3018f63f/include /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pytorch-cluster/1.6.3-d29ee1aa75a636f15d60189c3018f63f/include/torchcluster/
pytorch-cluster_EX_LIB := torchcluster
pytorch-cluster_EX_USE := pytorch

ALL_TOOLS      += pytorch-cuda
pytorch-cuda_EX_LIB := torch_cuda c10_cuda
pytorch-cuda_EX_USE := cuda torch-interface

ALL_TOOLS      += pytorch-custom-ops
pytorch-custom-ops_EX_USE := pytorch pytorch-scatter pytorch-cluster pytorch-sparse

ALL_TOOLS      += pytorch-scatter
pytorch-scatter_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pytorch-scatter/2.1.2-0f4d37849e0d72e715ba71291d0d96f9/include /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pytorch-scatter/2.1.2-0f4d37849e0d72e715ba71291d0d96f9/include/torchscatter/
pytorch-scatter_EX_LIB := torchscatter
pytorch-scatter_EX_USE := pytorch

ALL_TOOLS      += pytorch-sparse
pytorch-sparse_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pytorch-sparse/0.6.18-b31d071602817a93e8e6ab6c8656957b/include /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pytorch-sparse/0.6.18-b31d071602817a93e8e6ab6c8656957b/include/torchsparse/
pytorch-sparse_EX_LIB := torchsparse
pytorch-sparse_EX_USE := pytorch

ALL_TOOLS      += pytorch
pytorch_EX_LIB := torch torch_cpu c10
pytorch_EX_USE := torch-interface

ALL_TOOLS      += pytorch_x86-64-v3

ALL_TOOLS      += qd
qd_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/qd/2.3.13-869155c589873939757066b8f1c8a798/include
qd_EX_LIB := qdmod qd

ALL_TOOLS      += qd_f_main
qd_f_main_EX_LIB := qd_f_main
qd_f_main_EX_USE := qd

ALL_TOOLS      += rdma-core
rdma-core_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/rdma-core/50.0-a6279657e195ef15d5fac29e68a78183/include

ALL_TOOLS      += re2
re2_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/re2/2021-06-01-52c3298079c0345449c4fba721c307db/include
re2_EX_LIB := re2

ALL_TOOLS      += rivet
rivet_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/rivet/3.1.10-bd87a2bb3887f5a0f571abd029577d3f/include
rivet_EX_LIB := Rivet
rivet_EX_USE := hepmc fastjet fastjet-contrib gsl yoda

ALL_TOOLS      += rivet_x86-64-v3

ALL_TOOLS      += rocm-rocrand
rocm-rocrand_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/rocm-rocrand/5.6.1-d770d75dd2e6b33d79ed9c0055edf9a5/include
rocm-rocrand_EX_LIB := hiprand rocrand
rocm-rocrand_EX_USE := rocm
rocm-rocrand_EX_FLAGS_SYSTEM_INCLUDE  := 1

ALL_TOOLS      += rocm
rocm_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/rocm/5.6.1-3963e5b385e239849e4ae2ca3af3a386/include
rocm_EX_LIB := amdhip64 hsa-runtime64 rocm_smi64
rocm_EX_FLAGS_CPPDEFINES  := -D__HIP_PLATFORM_HCC__ -D__HIP_PLATFORM_AMD__
rocm_EX_FLAGS_ROCM_FLAGS  := -fgpu-rdc --offload-arch=gfx900 --offload-arch=gfx906 --offload-arch=gfx908 --offload-arch=gfx90a --offload-arch=gfx1030 --target=x86_64-redhat-linux-gnu --gcc-toolchain=$(COMPILER_PATH)
rocm_EX_FLAGS_SYSTEM_INCLUDE  := 1

ALL_TOOLS      += roofit
roofit_EX_LIB := RooFit
roofit_EX_USE := roofitcore rootcore rootmath roothistmatrix

ALL_TOOLS      += roofitcore
roofitcore_EX_LIB := RooFitCore
roofitcore_EX_USE := rootcore roothistmatrix rootgpad rootminuit root_cxxdefaults

ALL_TOOLS      += roostats
roostats_EX_LIB := RooStats
roostats_EX_USE := roofitcore roofit rootcore roothistmatrix rootgpad

ALL_TOOLS      += root
root_EX_USE := rootphysics
root_EX_FLAGS_CXXMODULES  := 0
root_EX_FLAGS_GENREFLEX_FAILES_ON_WARNS  := -failOnWarnings

ALL_TOOLS      += root_cxxdefaults

ALL_TOOLS      += root_interface
root_interface_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/lcg/root/6.30.07-8b1a11e1ef0e074fdfd44e162b27e71c/include
root_interface_EX_USE := root_cxxdefaults
root_interface_EX_FLAGS_SYSTEM_INCLUDE  := 1

ALL_TOOLS      += rootcling
rootcling_EX_LIB := Core
rootcling_EX_USE := root_interface sockets pcre zlib xz
TOOLS_OVERRIDABLE_FLAGS  +=ROOTCLING_ARGS

ALL_TOOLS      += rootcore
rootcore_EX_LIB := Tree Net
rootcore_EX_USE := rootmathcore rootthread

ALL_TOOLS      += rootdataframe
rootdataframe_EX_LIB := ROOTDataFrame
rootdataframe_EX_USE := rootcore rootgraphics roothistmatrix rootrio rootvecops

ALL_TOOLS      += rooteg
rooteg_EX_LIB := EG
rooteg_EX_USE := rootgraphics

ALL_TOOLS      += rooteve
rooteve_EX_LIB := Eve
rooteve_EX_USE := rootgeompainter rootrgl rootged

ALL_TOOLS      += rootfoam
rootfoam_EX_LIB := Foam
rootfoam_EX_USE := roothistmatrix

ALL_TOOLS      += rootged
rootged_EX_LIB := Ged
rootged_EX_USE := rootgui

ALL_TOOLS      += rootgeom
rootgeom_EX_LIB := Geom
rootgeom_EX_USE := rootrio rootmathcore

ALL_TOOLS      += rootgeompainter
rootgeompainter_EX_LIB := GeomPainter
rootgeompainter_EX_USE := rootgeom rootgraphics

ALL_TOOLS      += rootglew
rootglew_EX_LIB := GLEW

ALL_TOOLS      += rootgpad
rootgpad_EX_LIB := Gpad Graf
rootgpad_EX_USE := roothistmatrix

ALL_TOOLS      += rootgraphics
rootgraphics_EX_LIB := TreePlayer Graf3d Postscript
rootgraphics_EX_USE := rootgpad

ALL_TOOLS      += rootgui
rootgui_EX_LIB := Gui
rootgui_EX_USE := rootgpad

ALL_TOOLS      += rootguihtml
rootguihtml_EX_LIB := GuiHtml
rootguihtml_EX_USE := rootgui rootinteractive

ALL_TOOLS      += roothistmatrix
roothistmatrix_EX_LIB := Hist Matrix
roothistmatrix_EX_USE := rootcore

ALL_TOOLS      += roothistpainter
roothistpainter_EX_LIB := HistPainter
roothistpainter_EX_USE := roothistmatrix rootgpad rootimt

ALL_TOOLS      += roothtml
roothtml_EX_LIB := Html
roothtml_EX_USE := rootgpad

ALL_TOOLS      += rootimt
rootimt_EX_LIB := Imt
rootimt_EX_USE := rootthread tbb

ALL_TOOLS      += rootinteractive
rootinteractive_EX_LIB := Gui
rootinteractive_EX_USE := libjpeg-turbo libpng rootgpad rootrint

ALL_TOOLS      += rootmath
rootmath_EX_LIB := GenVector MathMore
rootmath_EX_USE := rootcore gsl

ALL_TOOLS      += rootmathcore
rootmathcore_EX_LIB := MathCore
rootmathcore_EX_USE := rootcling

ALL_TOOLS      += rootminuit
rootminuit_EX_LIB := Minuit
rootminuit_EX_USE := rootgpad

ALL_TOOLS      += rootminuit2
rootminuit2_EX_LIB := Minuit2
rootminuit2_EX_USE := rootgpad

ALL_TOOLS      += rootmlp
rootmlp_EX_LIB := MLP
rootmlp_EX_USE := rootgraphics

ALL_TOOLS      += rootntuple
rootntuple_EX_LIB := ROOTNTuple
rootntuple_EX_USE := rootvecops rootthread

ALL_TOOLS      += rootphysics
rootphysics_EX_LIB := Physics
rootphysics_EX_USE := roothistmatrix

ALL_TOOLS      += rootpy
rootpy_EX_USE := rootgraphics

ALL_TOOLS      += rootpymva
rootpymva_EX_LIB := PyMVA
rootpymva_EX_USE := roottmva numpy-c-api

ALL_TOOLS      += rootrflx
rootrflx_EX_USE := root_interface rootcling
rootrflx_EX_FLAGS_GENREFLEX_CPPFLAGS  := -DCMS_DICT_IMPL -D_REENTRANT -DGNUSOURCE -D__STRICT_ANSI__
rootrflx_EX_FLAGS_GENREFLEX_GCCXMLOPT  := -m64
TOOLS_OVERRIDABLE_FLAGS  +=GENREFLEX_CPPFLAGS

ALL_TOOLS      += rootrgl
rootrgl_EX_LIB := RGL
rootrgl_EX_USE := rootglew rootgui rootinteractive rootgraphics

ALL_TOOLS      += rootrint
rootrint_EX_LIB := Rint
rootrint_EX_USE := rootcling

ALL_TOOLS      += rootrio
rootrio_EX_LIB := RIO
rootrio_EX_USE := rootcling

ALL_TOOLS      += rootsmatrix
rootsmatrix_EX_LIB := Smatrix
rootsmatrix_EX_USE := rootcling

ALL_TOOLS      += rootspectrum
rootspectrum_EX_LIB := Spectrum
rootspectrum_EX_USE := roothistmatrix

ALL_TOOLS      += rootthread
rootthread_EX_LIB := Thread
rootthread_EX_USE := rootrio

ALL_TOOLS      += roottmva
roottmva_EX_LIB := TMVA
roottmva_EX_USE := rootmlp rootminuit

ALL_TOOLS      += rootvecops
rootvecops_EX_LIB := ROOTVecOps
rootvecops_EX_USE := rootcore

ALL_TOOLS      += rootx11
rootx11_EX_LIB := GX11
rootx11_EX_USE := rootcling

ALL_TOOLS      += rootxml
rootxml_EX_LIB := XMLParser
rootxml_EX_USE := rootcore libxml2

ALL_TOOLS      += rootxmlio
rootxmlio_EX_LIB := XMLIO
rootxmlio_EX_USE := rootrio

ALL_TOOLS      += sanitizer-flags-asan

ALL_TOOLS      += sanitizer-flags-ubsan

ALL_TOOLS      += self
self_EX_INCLUDE := /eos/home-m/mblancco/tau_analysis/MuTau_channel/datacards/CMSSW_14_1_0_pre4/src /eos/home-m/mblancco/tau_analysis/MuTau_channel/datacards/CMSSW_14_1_0_pre4/include/el9_amd64_gcc12/src /eos/home-m/mblancco/tau_analysis/MuTau_channel/datacards/CMSSW_14_1_0_pre4/include/LCG /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_14_1_0_pre4/src
self_EX_LIBDIR := /eos/home-m/mblancco/tau_analysis/MuTau_channel/datacards/CMSSW_14_1_0_pre4/biglib/el9_amd64_gcc12 /eos/home-m/mblancco/tau_analysis/MuTau_channel/datacards/CMSSW_14_1_0_pre4/lib/el9_amd64_gcc12 /eos/home-m/mblancco/tau_analysis/MuTau_channel/datacards/CMSSW_14_1_0_pre4/external/el9_amd64_gcc12/lib /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_14_1_0_pre4/biglib/el9_amd64_gcc12 /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_14_1_0_pre4/lib/el9_amd64_gcc12 /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_14_1_0_pre4/external/el9_amd64_gcc12/lib
self_EX_FLAGS_ALPAKA_BACKENDS  := cuda rocm serial
self_EX_FLAGS_CHECK_PRIVATE_HEADERS  := 1
self_EX_FLAGS_CODE_CHECK_ALPAKA_BACKEND  := serial
self_EX_FLAGS_DEFAULT_COMPILER  := gcc
self_EX_FLAGS_ENABLE_LTO  := 1
self_EX_FLAGS_ENABLE_PGO  := 0
self_EX_FLAGS_EXTERNAL_SYMLINK  := PATH LIBDIR CMSSW_SEARCH_PATH
self_EX_FLAGS_LLVM_ANALYZER  := llvm-analyzer
self_EX_FLAGS_NO_EXTERNAL_RUNTIME  := LD_LIBRARY_PATH PATH CMSSW_SEARCH_PATH
TOOLS_OVERRIDABLE_FLAGS  +=CPPDEFINES CXXFLAGS FFLAGS CFLAGS CPPFLAGS LDFLAGS CUDA_FLAGS CUDA_LDFLAGS LTO_FLAGS PGO_FLAGS ROCM_FLAGS ROCM_LDFLAGS
self_EX_FLAGS_SCRAM_TARGETS  := x86-64-v3
self_EX_FLAGS_SKIP_TOOLS_SYMLINK  := cxxcompiler ccompiler f77compiler gcc-cxxcompiler gcc-ccompiler gcc-f77compiler llvm-cxxcompiler llvm-ccompiler llvm-f77compiler llvm-analyzer-cxxcompiler llvm-analyzer-ccompiler icc-cxxcompiler icc-ccompiler icc-f77compiler x11 dpm
self_EX_FLAGS_SYMLINK_DEPTH_CMSSW_SEARCH_PATH  := 2
self_ORDER := 20000
IS_PATCH:=

ALL_TOOLS      += sherpa
sherpa_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/sherpa/2.2.15-eb7c608acd014832726c76d5379894b0/include/SHERPA-MC
sherpa_EX_LIB := SherpaMain ToolsMath ToolsOrg
sherpa_EX_USE := root_cxxdefaults hepmc lhapdf qd blackhat fastjet sqlite openmpi openloops

ALL_TOOLS      += sigcpp
sigcpp_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/sigcpp/3.2.0-1d8286563e3e21dce7bc52c496afc741/include/sigc++-3.0
sigcpp_EX_LIB := sigc-3.0
sigcpp_EX_USE := root_cxxdefaults

ALL_TOOLS      += sloccount

ALL_TOOLS      += sockets
sockets_EX_LIB := crypt dl rt

ALL_TOOLS      += sqlite
sqlite_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/sqlite/3.36.0-49157ebb9a846c99702be39683b0c263/include
sqlite_EX_LIB := sqlite3
sqlite_EX_USE := root_cxxdefaults

ALL_TOOLS      += starlight
starlight_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/starlight/r193-c91b05bd109ef77fd38ac6c8ee3b8cb1/include
starlight_EX_LIB := Starlib
starlight_EX_USE := root_cxxdefaults clhep

ALL_TOOLS      += stdcxx-fs
stdcxx-fs_EX_LIB := stdc++fs

ALL_TOOLS      += tauolapp
tauolapp_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tauolapp/1.1.8-f25a50d3adb7b2a2ca0776796b4fef8b/include
tauolapp_EX_LIB := TauolaCxxInterface TauolaFortran TauolaTauSpinner TauolaHepMC TauolaHEPEVT
tauolapp_EX_USE := root_cxxdefaults hepmc f77compiler pythia8 lhapdf

ALL_TOOLS      += tbb
tbb_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tbb/v2021.9.0-7e31cf78e4a7495ff0337c859d1dc314/include
tbb_EX_LIB := tbb
tbb_EX_USE := root_cxxdefaults
tbb_EX_FLAGS_CPPDEFINES  := -DTBB_USE_GLIBCXX_VERSION=120301 -DTBB_SUPPRESS_DEPRECATED_MESSAGES -DTBB_PREVIEW_RESUMABLE_TASKS=1 -DTBB_PREVIEW_TASK_GROUP_EXTENSIONS=1
tbb_EX_FLAGS_SYSTEM_INCLUDE  := 1

ALL_TOOLS      += tbbbind
tbbbind_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tbb/v2021.9.0-7e31cf78e4a7495ff0337c859d1dc314/include
tbbbind_EX_LIB := tbbbind_2_0
tbbbind_EX_USE := tbb
tbbbind_EX_FLAGS_SYSTEM_INCLUDE  := 1

ALL_TOOLS      += tcmalloc
tcmalloc_EX_LIB := tcmalloc

ALL_TOOLS      += tcmalloc_minimal
tcmalloc_minimal_EX_LIB := tcmalloc_minimal

ALL_TOOLS      += tensorflow-c
tensorflow-c_EX_LIB := tensorflow
tensorflow-c_EX_USE := tensorflow-framework eigen libpng sqlite

ALL_TOOLS      += tensorflow-cc
tensorflow-cc_EX_LIB := tensorflow_cc
tensorflow-cc_EX_USE := tensorflow-framework eigen libpng sqlite

ALL_TOOLS      += tensorflow-executable_run_options
tensorflow-executable_run_options_EX_LIB := executable_run_options
tensorflow-executable_run_options_EX_USE := tensorflow

ALL_TOOLS      += tensorflow-framework
tensorflow-framework_EX_LIB := tensorflow_framework
tensorflow-framework_EX_USE := tensorflow giflib zlib libjpeg-turbo protobuf

ALL_TOOLS      += tensorflow-includes
tensorflow-includes_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/py3-tensorflow/2.12.0-99b601608ae4457d0b7025a405a58d81/lib/python3.9/site-packages/tensorflow/include

ALL_TOOLS      += tensorflow-runtime
tensorflow-runtime_EX_LIB := cpu_function_runtime
tensorflow-runtime_EX_USE := tensorflow

ALL_TOOLS      += tensorflow-tf2xla
tensorflow-tf2xla_EX_LIB := tf2xla
tensorflow-tf2xla_EX_USE := tensorflow

ALL_TOOLS      += tensorflow-xla-runtime
tensorflow-xla-runtime_EX_LIB := tf_xla_runtime
tensorflow-xla-runtime_EX_USE := eigen tensorflow-includes

ALL_TOOLS      += tensorflow-xla_compiled_cpu_function
tensorflow-xla_compiled_cpu_function_EX_LIB := xla_compiled_cpu_function
tensorflow-xla_compiled_cpu_function_EX_USE := tensorflow

ALL_TOOLS      += tensorflow
tensorflow_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tensorflow/2.12.0-d43c187db3b5d7f2fb1b49d0e3cc6d3d/include
tensorflow_EX_FLAGS_SYSTEM_INCLUDE  := 1

ALL_TOOLS      += tensorflow_x86-64-v3

ALL_TOOLS      += tfaot-model-test-multi
tfaot-model-test-multi_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tfaot-model-test-multi/1.0.1-cc0367984c8b9ba39db9b8fb2aee2ff6/include
tfaot-model-test-multi_EX_USE := tensorflow-xla-runtime
tfaot-model-test-multi_EX_FLAGS_LDFLAGS  := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tfaot-model-test-multi/1.0.1-cc0367984c8b9ba39db9b8fb2aee2ff6/lib/test_multi_bs1.o /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tfaot-model-test-multi/1.0.1-cc0367984c8b9ba39db9b8fb2aee2ff6/lib/test_multi_bs2.o /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tfaot-model-test-multi/1.0.1-cc0367984c8b9ba39db9b8fb2aee2ff6/lib/test_multi_bs4.o

ALL_TOOLS      += tfaot-model-test-simple
tfaot-model-test-simple_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tfaot-model-test-simple/1.0.1-4593a8816f688e54be28b5d1f49c913e/include
tfaot-model-test-simple_EX_USE := tensorflow-xla-runtime
tfaot-model-test-simple_EX_FLAGS_LDFLAGS  := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tfaot-model-test-simple/1.0.1-4593a8816f688e54be28b5d1f49c913e/lib/test_simple_bs1.o /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tfaot-model-test-simple/1.0.1-4593a8816f688e54be28b5d1f49c913e/lib/test_simple_bs2.o /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tfaot-model-test-simple/1.0.1-4593a8816f688e54be28b5d1f49c913e/lib/test_simple_bs4.o

ALL_TOOLS      += thepeg
thepeg_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/thepeg/2.2.2-39d7bb6acb39a1a428e461d52998537e/include
thepeg_EX_LIB := ThePEG LesHouches
thepeg_EX_USE := root_cxxdefaults lhapdf gsl

ALL_TOOLS      += tinyxml2
tinyxml2_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tinyxml2/6.2.0-88fe0ec301baf763fa3c485e5b67ed91/include
tinyxml2_EX_LIB := tinyxml2

ALL_TOOLS      += tkonlinesw
tkonlinesw_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/tkonlinesw/4.2.0-1_gcc7-2f44505576c8374a21eedf84873956d4/include
tkonlinesw_EX_LIB := ICUtils Fed9UUtils
tkonlinesw_EX_USE := root_cxxdefaults xerces-c
tkonlinesw_EX_FLAGS_CXXFLAGS  := -DCMS_TK_64BITS

ALL_TOOLS      += tkonlineswdb
tkonlineswdb_EX_LIB := DeviceDescriptions Fed9UDeviceFactory
tkonlineswdb_EX_USE := tkonlinesw oracle oracleocci

ALL_TOOLS      += toollip

ALL_TOOLS      += torch-interface
torch-interface_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pytorch/2.1.1-0688c5edacc687489952b87ce07ffb8e/include /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/pytorch/2.1.1-0688c5edacc687489952b87ce07ffb8e/include/torch/csrc/api/include
torch-interface_EX_USE := protobuf zlib

ALL_TOOLS      += triton-inference-client
triton-inference-client_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/triton-inference-client/2.25.0-de33d7fa2454e8b619083fbcfe1cbe6b/include
triton-inference-client_EX_LIB := grpcclient tritoncommonmodelconfig
triton-inference-client_EX_USE := protobuf grpc cuda re2

ALL_TOOLS      += ucx
ucx_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/ucx/1.15.0-918a2ecd367f2cbcad966ee0bf07acfc/include
ucx_EX_LIB := ucp uct ucs ucm

ALL_TOOLS      += utm
utm_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/utm/utm_0.12.0-9791fe37faeadd79cf1fb1a19cb0745d/include
utm_EX_LIB := tmeventsetup tmtable tmxsd tmgrammar tmutil
utm_EX_USE := root_cxxdefaults

ALL_TOOLS      += valgrind
valgrind_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/valgrind/3.23.0-bc5f1efe61963c7440bdcc46061745bc/include
valgrind_EX_USE := root_cxxdefaults

ALL_TOOLS      += vdt
vdt_EX_LIB := vdt
vdt_EX_USE := vdt_headers

ALL_TOOLS      += vdt_headers
vdt_headers_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/vdt/0.4.3-793cee1e1edef0e54b2bd5cb1f69aec9/include
vdt_headers_EX_USE := root_cxxdefaults

ALL_TOOLS      += vecgeom
vecgeom_EX_LIB := vecgeom
vecgeom_EX_USE := vecgeom_interface

ALL_TOOLS      += vecgeom_interface
vecgeom_interface_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/vecgeom/v1.2.7-a835f26582a68cd5d06d2fea9585a5f5/include /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/vecgeom/v1.2.7-a835f26582a68cd5d06d2fea9585a5f5/include/VecGeom
vecgeom_interface_EX_USE := root_cxxdefaults

ALL_TOOLS      += x11
x11_EX_USE := sockets

ALL_TOOLS      += xerces-c
xerces-c_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/xerces-c/3.1.3-c7b88eaa36d0408120f3c29826a04bf6/include
xerces-c_EX_LIB := xerces-c
xerces-c_EX_USE := root_cxxdefaults

ALL_TOOLS      += xgboost
xgboost_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/xgboost/1.7.5-d0dc099a6916fd0fc90fc77ce3b14aea/include
xgboost_EX_LIB := xgboost

ALL_TOOLS      += xpmem
xpmem_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/xpmem/v2.6.3-20220308-e317a33b47b8cbde38eda98c6a7ec259/include
xpmem_EX_LIB := xpmem

ALL_TOOLS      += xrdcl-record

ALL_TOOLS      += xrootd
xrootd_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/xrootd/5.6.4-3df5b85e979a403a275b28206295b011/include/xrootd /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/xrootd/5.6.4-3df5b85e979a403a275b28206295b011/include/xrootd/private
xrootd_EX_LIB := XrdUtils XrdCl
xrootd_EX_USE := root_cxxdefaults

ALL_TOOLS      += xtensor
xtensor_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/xtensor/0.24.1-d59140919e8188220087fa0f303266a3/include
xtensor_EX_USE := xtl

ALL_TOOLS      += xtl
xtl_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/xtl/0.7.4-ff8993f8412f3b6695988f2dd3b24012/include

ALL_TOOLS      += xz
xz_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/xz/5.2.5-87b9f5597eaeb8b5e9cedb5d183d5089/include
xz_EX_LIB := lzma
xz_EX_USE := root_cxxdefaults

ALL_TOOLS      += yaml-cpp
yaml-cpp_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/yaml-cpp/0.8.0-b7a093f6f712df85bedaa99de0082100/include
yaml-cpp_EX_LIB := yaml-cpp

ALL_TOOLS      += yoda
yoda_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/yoda/1.9.10-48e81cb14beb586f9567666c8ebd23ae/include
yoda_EX_LIB := YODA
yoda_EX_USE := root_cxxdefaults

ALL_TOOLS      += zlib
zlib_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/zlib/1.2.11-1a082fc322b0051b504cc023f21df178/include
zlib_EX_LIB := z
zlib_EX_USE := root_cxxdefaults

ALL_TOOLS      += zlib_x86-64-v3

ALL_TOOLS      += zstd
zstd_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/zstd/1.5.2-9fdadfca3fb3629e62141e0b4e73e88d/include
zstd_EX_LIB := zstd
zstd_EX_USE := root_cxxdefaults

