#include "qsslcertificateextension.h"
#include "qsslcertificateextension_p.h"

QT_BEGIN_NAMESPACE

QSslCertificateExtension::QSslCertificateExtension()
    : d(new QSslCertificateExtensionPrivate)
{
}

QSslCertificateExtension::QSslCertificateExtension(const QSslCertificateExtension &other)
    : d(other.d)
{
}

QSslCertificateExtension::~QSslCertificateExtension()
{
}

QSslCertificateExtension &QSslCertificateExtension::operator=(const QSslCertificateExtension &other)
{
    d = other.d;
    return *this;
}

QString QSslCertificateExtension::name() const
{
    return d->name;
}

QVariant QSslCertificateExtension::value() const
{
    return d->value;
}

bool QSslCertificateExtension::isCritical() const
{
    return d->critical;
}

bool QSslCertificateExtension::isSupported() const
{
    return d->supported;
}
