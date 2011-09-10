/*!
    \class QSslCertificateExtension
    \brief The QSslCertificateExtension provides an API for accessing the extensions of an X509 certificate.
    \since 5.0

    \rentrant
    \ingroup network
    \ingroup ssl
    \inmodule QtNetwork

    QSslCertificateExtension provides access to an extension stored in
    an X509 certificate. The information available depends on the type
    of extension being accessed.

    TODO: document the supported extensions etc.
    Whilst this class provides access to any type of extension, only
    some are guaranteed to be returned in a format that will remain
    unchanged between releases. The isSupported() method returns true
    for extensions where this is the case.
    
    Further information about the types of extensions certificates can
    contain can be found in RFC 5280.

    \sa QSslCertificate::extensions()
 */

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

/*!
    Returns the ASN.1 OID of this extension.
 */
QString QSslCertificateExtension::oid() const
{
    return d->oid;
}

/*!
    Returns the name of the extension. If no name is known for the
    extension then the OID will be returned.
 */
QString QSslCertificateExtension::name() const
{
    return d->name;
}

/*!
    Returns the value of the extension. The structure of the value
    returned depends on the extension type.
 */
QVariant QSslCertificateExtension::value() const
{
    return d->value;
}

/*!
    Returns the criticality of the extension.
 */
bool QSslCertificateExtension::isCritical() const
{
    return d->critical;
}

/*!
    Returns the true if this extension is supported. In this case,
    supported simply means that the structure of the QVariant returned
    by the value() accessor will remain unchanged between versions.
    Unsupported extensions can be freely used, however there is no
    guarantee that the returned data will have the same structure
    between versions.
 */
bool QSslCertificateExtension::isSupported() const
{
    return d->supported;
}
