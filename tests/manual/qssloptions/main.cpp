#include <QtNetwork/qsslconfiguration.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QTextStream>
#include <stdio.h>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    if (argc < 3) {
        QTextStream out(stdout);
        out << "Usage: " << argv[0] << " host port [options]" << endl;
        out << "The options can be one or more of the following:" << endl;
        out << "enable_empty_fragments" << endl;
        out << "disable_session_tickets" << endl;
        out << "disable_compression" << endl;
        out << "disable_sni" << endl;
        return 1;
    }

    QString host = QString::fromLocal8Bit(argv[1]);
    int port = QString::fromLocal8Bit(argv[2]).toInt();

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();

    for (int i=3; i < argc; i++) {
        QString option = QString::fromLocal8Bit(argv[i]);

        if (option == QStringLiteral("enable_empty_fragments"))
            config.setSslOption(QSsl::SslOptionDisableEmptyFragments, false);
        else if (option == QStringLiteral("disable_session_tickets"))
            config.setSslOption(QSsl::SslOptionDisableSessionTickets, true);
        else if (option == QStringLiteral("disable_compression"))
            config.setSslOption(QSsl::SslOptionDisableCompression, true);
        else if (option == QStringLiteral("disable_sni"))
            config.setSslOption(QSsl::SslOptionDisableServerNameIndication, true);
    }

    QSslConfiguration::setDefaultConfiguration(config);

    QSslSocket socket;
    //socket.setSslConfiguration(config);
    socket.connectToHostEncrypted(host, port);

    if ( !socket.waitForEncrypted() ) {
        qDebug() << socket.errorString();
        return 1;
    }

    return 0;
}
