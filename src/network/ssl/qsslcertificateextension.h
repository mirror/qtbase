#ifndef QSSLCERTIFICATEEXTENSION_H
#define QSSLCERTIFICATEEXTENSION_H

#include <QtCore/qnamespace.h>
#include <QtCore/qsharedpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Network)

#ifndef QT_NO_OPENSSL

class QSslCertificateExtensionPrivate;

class Q_NETWORK_EXPORT QSslCertificateExtension
{
public:
    QSslCertificateExtension();
    QSslCertificateExtension(const QSslCertificateExtension &other);
    ~QSslCertificateExtension();

    QSslCertificateExtension &operator=(const QSslCertificateExtension &other);

    QString oid() const;
    QString name() const;
    QVariant value() const;
    bool isCritical() const;

    bool isSupported() const;

private:
    friend class QSslCertificatePrivate;
    QExplicitlySharedDataPointer<QSslCertificateExtensionPrivate> d;
};

#endif // QT_NO_OPENSSL

QT_END_NAMESPACE

QT_END_HEADER


#endif // QSSLCERTIFICATEEXTENSION_H


