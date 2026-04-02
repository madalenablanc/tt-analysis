ALL_TOOLS      += dd4hep-core
dd4hep-core_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/dd4hep/v01-27-02-39caf811614f278f42bf52fb83e7c15c/include
dd4hep-core_EX_LIB := DDCore DDParsers
dd4hep-core_EX_USE := root_cxxdefaults root boost xerces-c clhep
dd4hep-core_EX_FLAGS_CPPDEFINES  := -DDD4HEP_USE_GEANT4_UNITS=1
dd4hep-core_EX_FLAGS_LISTCOMPONENTS  := $(DD4HEP_CORE_BASE)/bin/listcomponents_dd4hep

