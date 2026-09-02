#include "app/application.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    quickdeck::Application quickdeck_app;
    const quickdeck::Result<void> init_result = quickdeck_app.initialize();
    if (init_result.is_err()) {
        return 1;
    }
    return quickdeck_app.run();
}
