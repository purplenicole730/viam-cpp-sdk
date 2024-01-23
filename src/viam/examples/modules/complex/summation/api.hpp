// Note that `Summation` is implemented with `MySummation` in impl.hpp and impl.cpp.
#pragma once

#include <vector>

#include <grpcpp/channel.h>

#include <viam/sdk/common/utils.hpp>
#include <viam/sdk/config/resource.hpp>
#include <viam/sdk/registry/registry.hpp>
#include <viam/sdk/resource/resource_manager.hpp>
#include <viam/sdk/services/service.hpp>

#include "summation.grpc.pb.h"
#include "summation.pb.h"

using namespace viam::sdk;
using namespace viam::service::summation::v1;

// `SummationRegistration` defines a `ResourceRegistration` for the `Summation`
// service.
class SummationRegistration : public ResourceRegistration {
   public:
    explicit SummationRegistration(const google::protobuf::ServiceDescriptor* service_descriptor);
    std::shared_ptr<ResourceServer> create_resource_server(std::shared_ptr<ResourceManager> manager,
                                                           Server& server) override;
    std::shared_ptr<Resource> create_rpc_client(std::string name,
                                                std::shared_ptr<grpc::Channel> chan) override;
};

// A `Summation` is a custom modular service.
class Summation : public Service {
   public:
    // methods shared across all services
    static std::shared_ptr<ResourceRegistration> resource_registration();
    API api() const override;

    virtual double sum(std::vector<double> numbers) = 0;

   protected:
    explicit Summation(std::string name);
};

namespace viam::sdk {
template <>
struct API::traits<Summation> {
    static API api();
};
}  // namespace viam::sdk

// `SummationClient` is the gRPC client implementation of a `Summation`
// service.
class SummationClient : public Summation {
   public:
    SummationClient(std::string name, std::shared_ptr<grpc::Channel> channel);

    double sum(std::vector<double> numbers) override;

   private:
    using StubType = SummationService::StubInterface;
    std::unique_ptr<StubType> stub_;
    std::shared_ptr<grpc::Channel> channel_;
};

// `SummationServer` is the gRPC server implementation of a `Summation`
// service.
class SummationServer : public ResourceServer, public SummationService::Service {
   public:
    explicit SummationServer(std::shared_ptr<ResourceManager> manager);

    grpc::Status Sum(grpc::ServerContext* context,
                     const SumRequest* request,
                     SumResponse* response) override;
};
