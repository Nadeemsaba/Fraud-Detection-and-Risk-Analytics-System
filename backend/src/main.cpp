#include "database.h"
#include "server.h"

int main() {

    Database db;
    db.init();

    startServer(db);

    return 0;
}