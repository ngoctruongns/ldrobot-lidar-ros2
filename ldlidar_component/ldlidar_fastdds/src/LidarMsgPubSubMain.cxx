#include "LidarMsgPublisher.h"
#include "LidarMsgSubscriber.h"

int main(
        int argc,
        char** argv)
{
    int type = 0;

    if (argc == 2)
    {
        if (strcmp(argv[1], "publisher") == 0)
        {
            type = 1;
        }
        else if (strcmp(argv[1], "subscriber") == 0)
        {
            type = 2;
        }
    }

    if (type == 0)
    {
        std::cout << "Error: Incorrect arguments." << std::endl;
        std::cout << "Usage: " << std::endl << std::endl;
        std::cout << argv[0] << " publisher|subscriber" << std::endl << std::endl;
        return 0;
    }

    std::cout << "Starting " << std::endl;

    // Register the type being used

    switch (type)
    {
        case 1:
        {
            LidarMsgPublisher mypub;
            if (mypub.init())
            {
                mypub.run();
            }
            break;
        }
        case 2:
        {
            LidarMsgSubscriber mysub;
            if (mysub.init())
            {
                mysub.run();
            }
            break;
        }
    }

    return 0;
}
