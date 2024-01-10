// The MIT License(MIT)
// Copyright(c) <2016> <Juan Gonzalez Burgos>
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "qjsonwebtoken.h"


QJsonWebToken::QJsonWebToken():
    _payload(QJsonDocument::fromJson("{}"))
{
    // create the header with default algorithm
    setAlgorithmStr("HS256");
}

QJsonWebToken::QJsonWebToken(const QJsonWebToken &other)
{
    *this = other;
}

QJsonWebToken::QJsonWebToken(QJsonWebToken &&other)
{
    *this = std::move(other);
}

QJsonWebToken& QJsonWebToken::operator=(const QJsonWebToken &other)
{
    this->_header    = other._header;
    this->_payload   = other._payload;
    this->_signature = other._signature;
    this->_algorithm = other._algorithm;
    return *this;
}

QJsonWebToken& QJsonWebToken::operator=(QJsonWebToken &&other)
{
    this->_header = std::move(other._header);
    this->_payload = std::move(other._payload);
    this->_signature = std::move(other._signature);
    this->_algorithm = std::move(other._algorithm);
    return *this;
}

bool QJsonWebToken::operator==(const QJsonWebToken &other) const
{
    return _header == other._header &&
           _payload == other._payload &&
           _signature == other._signature &&
           _algorithm == other._algorithm;
}

QJsonDocument QJsonWebToken::getHeaderJDoc() const
{
    return _header;
}

QString QJsonWebToken::getHeaderQStr(const QJsonDocument::JsonFormat &format/* = QJsonDocument::JsonFormat::Indented*/) const
{
    return _header.toJson(format);
}

bool QJsonWebToken::setHeaderJDoc(const QJsonDocument &jdocHeader)
{
    if (jdocHeader.isEmpty() || jdocHeader.isNull() || !jdocHeader.isObject())
    {
        return false;
    }

    // check if supported algorithm
    QString strAlgorithm = jdocHeader.object().value("alg").toString("");
    if (!isAlgorithmSupported(strAlgorithm))
    {
        return false;
    }

    _header = jdocHeader;

    // set also new algorithm
    _algorithm = strAlgorithm;

    return true;
}

bool QJsonWebToken::setHeaderQStr(const QString &strHeader)
{
    QJsonParseError error;
    QJsonDocument tmpHeader = QJsonDocument::fromJson(strHeader.toUtf8(), &error);

    // validate and set header
    if (error.error != QJsonParseError::NoError || !setHeaderJDoc(tmpHeader))
    {
        return false;
    }

    return true;
}

QJsonDocument QJsonWebToken::getPayloadJDoc() const
{
    return _payload;
}

QString QJsonWebToken::getPayloadQStr(const QJsonDocument::JsonFormat &format/* = QJsonDocument::JsonFormat::Indented*/) const
{
    return _payload.toJson(format);
}

bool QJsonWebToken::setPayloadJDoc(const QJsonDocument &jdocPayload)
{
    if (jdocPayload.isEmpty() || jdocPayload.isNull() || !jdocPayload.isObject())
    {
        return false;
    }

    _payload = jdocPayload;

    return true;
}

bool QJsonWebToken::setPayloadQStr(const QString &strPayload)
{
    QJsonParseError error;
    QJsonDocument tmpPayload = QJsonDocument::fromJson(strPayload.toUtf8(), &error);

    // validate and set payload
    if (error.error != QJsonParseError::NoError || !setPayloadJDoc(tmpPayload))
    {
        return false;
    }

    return true;
}

QString QJsonWebToken::getAlgorithmStr() const
{
    return _algorithm;
}

bool QJsonWebToken::setAlgorithmStr(const QString &strAlgorithm)
{
    // check if supported algorithm
    if (!isAlgorithmSupported(strAlgorithm))
    {
        return false;
    }
    // set algorithm
    _algorithm = strAlgorithm;
    // modify header
    _header = QJsonDocument::fromJson(QByteArray("{\"typ\": \"JWT\", \"alg\" : \"")
                                         + _algorithm.toUtf8()
                                         + QByteArray("\"}"));

    return true;
}

QString QJsonWebToken::getToken() const noexcept
{
    return _token;
}

bool QJsonWebToken::setToken(const QString &strToken)
{
    _token = strToken;
    // assume base64 encoded at first, if not try decoding
    bool isBase64Encoded = true;
    QStringList listJwtParts = strToken.split(".");
    // check correct size
    if (listJwtParts.count() != 3)
    {
        return false;
    }
    // check all parts are valid using another instance,
    // so we dont overwrite this instance in case of error
    QJsonWebToken tempTokenObj;
    if ( !tempTokenObj.setHeaderQStr(QByteArray::fromBase64(listJwtParts.at(0).toUtf8(), QByteArray::Base64UrlEncoding)) ||
         !tempTokenObj.setPayloadQStr(QByteArray::fromBase64(listJwtParts.at(1).toUtf8(), QByteArray::Base64UrlEncoding)))
    {
        // try unencoded
        if (!tempTokenObj.setHeaderQStr(listJwtParts.at(0)) ||
            !tempTokenObj.setPayloadQStr(listJwtParts.at(1)))
        {
            return false;
        }
        else
        {
            isBase64Encoded = false;
        }
    }
    // set parts on this instance
    setHeaderQStr(tempTokenObj.getHeaderQStr());
    setPayloadQStr(tempTokenObj.getPayloadQStr());
    if (isBase64Encoded)
    { // unencode
        _signature = QByteArray::fromBase64(listJwtParts.at(2).toUtf8(),QByteArray::Base64UrlEncoding);
    }
    else
    {
        _signature = listJwtParts.at(2).toUtf8();
    }

    // success
    return true;
}

bool QJsonWebToken::isValid() const
{
    // calculate token on other instance,
    // so we dont overwrite this instance's signature
    QJsonWebToken tempTokenObj = *this;
    if (getExp() > QDateTime::currentDateTime().toSecsSinceEpoch())
    {
        return true;
    }

    return false;
}

QJsonWebToken QJsonWebToken::fromToken(const QString &strToken)
{
    QJsonWebToken tempTokenObj;
    // set Token
    tempTokenObj.setToken(strToken);
    // return
    return tempTokenObj;
}

void QJsonWebToken::appendClaim(const QString &strClaimType, const QString &strValue)
{
    // have to make a copy of the json object, modify the copy and then put it back, sigh
    QJsonObject jObj = _payload.object();
    jObj.insert(strClaimType, strValue);
    _payload = QJsonDocument(jObj);
}

void QJsonWebToken::removeClaim(const QString &strClaimType)
{
    // have to make a copy of the json object, modify the copy and then put it back, sigh
    QJsonObject jObj = _payload.object();
    jObj.remove(strClaimType);
    _payload = QJsonDocument(jObj);
}

QString QJsonWebToken::claim(const QString &strClaimType)
{
    QJsonObject jObj = _payload.object();

    return jObj[strClaimType].toString();
}

qint64 QJsonWebToken::getExp() const
{
    return _payload["exp"].toString().toLongLong();
}

QString QJsonWebToken::getUserName() const
{
    return _payload["username"].toString();
}

bool QJsonWebToken::isAlgorithmSupported(const QString &strAlgorithm)
{
    // TODO : support other algorithms
    if (strAlgorithm.compare("HS256", Qt::CaseSensitive) != 0 && // HMAC using SHA-256 hash algorithm
        strAlgorithm.compare("HS384", Qt::CaseSensitive) != 0 && // HMAC using SHA-384 hash algorithm
        strAlgorithm.compare("HS512", Qt::CaseSensitive) != 0 /*&& // HMAC using SHA-512 hash algorithm
        strAlgorithm.compare("RS256", Qt::CaseSensitive) != 0 && // RSA using SHA-256 hash algorithm
        strAlgorithm.compare("RS384", Qt::CaseSensitive) != 0 && // RSA using SHA-384 hash algorithm
        strAlgorithm.compare("RS512", Qt::CaseSensitive) != 0 && // RSA using SHA-512 hash algorithm
        strAlgorithm.compare("ES256", Qt::CaseSensitive) != 0 && // ECDSA using P-256 curve and SHA-256 hash algorithm
        strAlgorithm.compare("ES384", Qt::CaseSensitive) != 0 && // ECDSA using P-384 curve and SHA-384 hash algorithm
        strAlgorithm.compare("ES512", Qt::CaseSensitive) != 0*/)  // ECDSA using P-521 curve and SHA-512 hash algorithm
    {
        return false;
    }
    return true;
}

QStringList QJsonWebToken::supportedAlgorithms()
{
    // TODO : support other algorithms
    return QStringList() << "HS256" << "HS384" << "HS512";
}
