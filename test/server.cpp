#include <ucorf/server.h>
#include <ucorf/net_transport.h>
#include "demo.rpc.h"
#include <iostream>
#include <boost/smart_ptr/make_shared.hpp>
using std::cout;
using std::endl;
using namespace Echo;

struct MyEcho : public ::Echo::UcorfEchoService
{
    virtual bool Echo(EchoRequest & request, EchoResponse & response)
    {
        cout << "Handle Echo, code=" << request.code() << endl;
        response.set_code(request.code());
        return true;
    }
};

int main(int argc, char **argv)
{
    using namespace ucorf;

    const char* url = "tcp://127.0.0.1:8080";
    if (argc > 1)
        url = argv[1];

    auto header_factory = [] {
        return boost::static_pointer_cast<IHeader>(boost::make_shared<UcorfHead>());
    };

    std::unique_ptr<NetTransportServer> tp(new NetTransportServer);
    boost_ec ec = tp->Listen(url);
    if (ec) {
        cout << "listen " << url << " error: " << ec.message() << endl;
        return 1;
    } else {
        cout << "start success: " << url << endl;
    }

    std::shared_ptr<IService> echo_srv(new MyEcho);
    Server server;
    server.BindTransport(std::move(tp)).SetHeaderFactory(header_factory)
        .RegisterService(echo_srv);

    co_sched.RunLoop();
    return 0;
}