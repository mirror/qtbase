#ifndef QSSLCERTIFICATEEXTESNION_P_H
#define QSSLCERTIFICATEEXTESNION_P_H

#include "qsslcertificateextension.h"

QT_BEGIN_NAMESPACE

class QSslCertificateExtensionPrivate
{
public:
    inline QSslCertificateExtensionPrivate()
        : critical(false),
          supported(false)
    {
    }

    QString oid;
    QString name;
    QVariant value;
    bool critical;
    bool supported;

    QAtomicInt ref;

private:
    Q_DISABLE_COPY(QSslCertificateExtensionPrivate)
};

QT_END_NAMESPACE

#endif // QSSLCERTIFICATEEXTESNION_P_H

