#include <iostream>
#include "base/at_exit.h"
#include "webdriver_server.h"
#include "webdriver_view_transitions.h"
#include "versioninfo.h"
#include "webdriver_route_table.h"
#include "shutdown_command.h"
#include "webdriver_route_patterns.h"
#include "extension_wpe/uinput_manager.h"
#include "extension_wpe/wd_event_dispatcher.h"
#include "extension_wpe/wpe_view_creator.h"
#include "extension_wpe/wpe_view_creator.h"
#include "extension_wpe/wpe_view_enumerator.h"
#include "extension_wpe/wpe_view_executor.h"

#include "webdriver_switches.h"
#include <glib.h>

void PrintVersion();
void PrintHelp();
void InitUInputClient();

int main(int argc, char *argv[])
{
    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    base::AtExitManager exit;
    CommandLine cmd_line(CommandLine::NO_PROGRAM);
    cmd_line.InitFromArgv(argc, argv);

    // WPE Webkit View
    webdriver::ViewCreator* wpeViewCreator = new webdriver::WpeViewCreator();
    wpeViewCreator->RegisterViewClass<WpeWidget>("WpeWidget");
    webdriver::ViewFactory::GetInstance()->AddViewCreator(wpeViewCreator);

    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WpeViewEnumeratorImpl());

    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::WpeViewCmdExecutorCreator());

    /* Parse command line */
    webdriver::Server* wd_server = webdriver::Server::GetInstance();
    if (0 != wd_server->Configure(cmd_line)) {
        std::cout << "Error while configuring WD server, exiting..." << std::endl;
        return 1;
    }
    std::cout << "Got webdriver server instance..." << std::endl;

    /* Example how to add a custom command */
    webdriver::RouteTable *routeTableWithShutdownCommand = new webdriver::RouteTable(wd_server->GetRouteTable());
    const char shutdownCommandRoute[] = "/-cisco-shutdown";
    routeTableWithShutdownCommand->Add<webdriver::ShutdownCommand>(shutdownCommandRoute);
    routeTableWithShutdownCommand->Add<webdriver::ShutdownCommand>(webdriver::CommandRoutes::kShutdown);
    wd_server->SetRouteTable(routeTableWithShutdownCommand);

    //    InitUInputClient();

    /* Start webdriver */
    int startError = wd_server->Start();
    if (startError){
        std::cout << "Error while starting server, errorCode " << startError << std::endl;
        return startError;
    }
    std::cout << "webdriver server started..." << std::endl;

    g_main_loop_run(loop);
    g_main_loop_unref(loop);
    return 0; 
}


void InitUInputClient() {
    // start user input device
    CommandLine cmdLine = webdriver::Server::GetInstance()->GetCommandLine();
    if (cmdLine.HasSwitch(webdriver::Switches::kUserInputDevice))
    {
        UInputManager *manager = UInputManager::getInstance();
        if (!manager->isReady())
        {
            manager->registerUinputDevice();
        }

//        WDEventDispatcher::getInstance()->add(new UInputEventDispatcher(manager));
    }
}

void PrintVersion() {
    std::cout <<webdriver::VersionInfo::CreateVersionString()<< std::endl;
}

void PrintHelp() {
    std::cout << "Usage: WebDriver [--OPTION=VALUE]..."                                             << std::endl

                << "Starts WPEWebDriver server"                                                       << std::endl
                << ""                                                                                 << std::endl
                << "OPTION         DEFAULT VALUE      DESCRIPTION"                                    << std::endl
                << "http-threads   4                  The number of threads to use for handling"      << std::endl
                << "                                  HTTP requests"                                  << std::endl
                << "log-path       ./webdriver.log    The path to use for the WPEWebDriver server"    << std::endl
                << "                                  log"                                            << std::endl
                << "root           ./web              The path of location to serve files from"       << std::endl
                << "port           9517               The port that WPEWebDriver listens on"          << std::endl
                << "silence        false              If true, WPEWebDriver will not log anything"    << std::endl
                << "                                  to stdout/stderr"                               << std::endl
                << "verbose        false              If true, WPEWebDriver will log lots of stuff"   << std::endl
                << "                                  to stdout/stderr"                               << std::endl
                << "url-base                          The URL path prefix to use for all incoming"    << std::endl
                << "                                  WebDriver REST requests. A prefix and postfix"  << std::endl
                << "                                  '/' will automatically be appended if not"      << std::endl
                << "                                  present"                                        << std::endl
                << "config                            The path to config file (e.g. config.json) in"  << std::endl
                << "                                  JSON format with specified WD parameters as"    << std::endl
                << "                                  described above (port, root, etc.)"             << std::endl
                << "wi-server      false              If true, web inspector will be enabled"         << std::endl
                << "wi-port        9222               Web inspector listening port"                   << std::endl
                << "version                           Print version information to stdout and exit"   << std::endl
                << "vnc-login      127.0.0.1:5900     VNC server login parameters"                    << std::endl
                << "                                  format: login:password@ip:port"                 << std::endl
                << "uinput         false              If option set, user input device"               << std::endl
                << "                                  will be registered in the system"               << std::endl
                << "test_data      ./                 Specifies where to look for test specific data" << std::endl
                << "whitelist                         The path to whitelist file (e.g. whitelist.xml)"<< std::endl
                << "                                  in XML format with specified list of IP with"   << std::endl
                << "                                  allowed/denied commands for each of them."      << std::endl
                << "webserver-cfg                     The path to mongoose config file"               << std::endl
                << "                                  (e.g. /path/to/config.json) in JSON format with"<< std::endl
                << "                                  specified mongoose start option"                << std::endl
                << "                                  (extra-mime-types, listening_ports, etc.)"      << std::endl
                << "                                  Option from webserver config file will have"    << std::endl
                << "                                  more priority than commandline param"           << std::endl
                << "                                  that specify the same option."                  << std::endl;
}



