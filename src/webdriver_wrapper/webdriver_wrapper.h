using namespace std;

namespace webdriver{

typedef enum {
    WD_CONFGIURED,
    WD_UNCONFIGURED,
    WD_RUNNING
    }WDStatus;

class WebdriverWrapper
{
public:
      WDStatus state_;
      WebdriverWrapper();
      ~WebdriverWrapper();
      int RunWebdriver();
      int IsWebdriverRunning();
};

}//namespace webdriver
