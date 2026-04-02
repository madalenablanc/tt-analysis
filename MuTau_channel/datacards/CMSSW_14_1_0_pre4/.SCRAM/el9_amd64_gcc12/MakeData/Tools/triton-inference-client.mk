ALL_TOOLS      += triton-inference-client
triton-inference-client_EX_INCLUDE := /cvmfs/cms.cern.ch/el9_amd64_gcc12/external/triton-inference-client/2.25.0-de33d7fa2454e8b619083fbcfe1cbe6b/include
triton-inference-client_EX_LIB := grpcclient tritoncommonmodelconfig
triton-inference-client_EX_USE := protobuf grpc cuda re2

