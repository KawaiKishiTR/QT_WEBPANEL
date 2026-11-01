#pragma once
#include <QString>
#include <QCoreApplication>
#include <QFileInfo>


namespace Config {
// updater app secret key
const inline QString ENV_KEY = "LAUNCHED_BY";
const inline QString SECRET_KEY = "917f74eee56d2e89d67066768bef577de011586d33e563ad3cb59864374a1406";

struct AppConfig {
    QString SERVER_HOST = "80.208.221.192";
    QString SERVER_PORT = "8000";
    QString API_KEY = "3580e7947da360248c93d4984d080301b4b379479fc5124bd144daf6cffb71b0";

};

static AppConfig APP_CONFIG;

QString get_running_dir();
inline QString get_config_file() {return get_running_dir() + "/APP.CONF.BIN";}
inline quint32 get_config_magic() {return 0x51544346;}
inline QString get_url() {return "http://" + APP_CONFIG.SERVER_HOST + ":" + APP_CONFIG.SERVER_PORT;}

bool saveBinaryConfig();
bool loadBinaryConfig();

} // namespace Config