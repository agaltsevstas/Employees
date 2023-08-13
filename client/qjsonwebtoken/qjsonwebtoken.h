/**
\file
\version 1.0
\date    22/06/2016
\author  JGB
\brief   JWT (JSON Web Token) Implementation in Qt C++
*/

// The MIT License(MIT)
// Copyright(c) <2016> <Juan Gonzalez Burgos>
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef QJSONWEBTOKEN_H
#define QJSONWEBTOKEN_H

#include <QMessageAuthenticationCode>
#include <QJsonDocument>
#include <QJsonObject>

/**

\brief   QJsonWebToken : JWT (JSON Web Token) Implementation in Qt C++

## Introduction

This class implements a subset of the [JSON Web Token](https://en.wikipedia.org/wiki/JSON_Web_Token) 
open standard [RFC 7519](https://tools.ietf.org/html/rfc7519).

Currently this implementation only supports the following algorithms:

Alg   | Parameter Value	Algorithm
----- | ------------------------------------
HS256 | HMAC using SHA-256 hash algorithm
HS384 | HMAC using SHA-384 hash algorithm
HS512 | HMAC using SHA-512 hash algorithm

*/


class Q_DECL_EXPORT QJsonWebToken
{

public:

    /**

    \brief Constructor.
    \return A new instance of QJsonWebToken.

    Creates a default QJsonWebToken instance with *HS256 algorithm*, empty *payload*
    and empty *secret*.

    */
    QJsonWebToken();

    /**

    \brief Copy Construtor.
    \param other Other QJsonWebToken to copy from.
    \return A new instance of QJsonWebToken with same contents as the *other* instance.

    Copies to the new instance the JWT *header*, *payload*, *signature*, *secret* and *algorithm*.

    */
    QJsonWebToken(const QJsonWebToken &other);

    QJsonWebToken(QJsonWebToken &&other);

    QJsonWebToken& operator=(const QJsonWebToken &other);

    QJsonWebToken& operator=(QJsonWebToken &&other);

    bool operator==(const QJsonWebToken &other) const;

    /**

    \brief Returns the JWT *header* as a QJsonDocument.
    \return JWT *header* as a QJsonDocument.

    */
    QJsonDocument getHeaderJDoc() const;

    /**

    \brief Returns the JWT *header* as a QString.
    \param format Defines the format of the JSON returned.
    \return JWT *header* as a QString.

    Format can be *QJsonDocument::JsonFormat::Indented* or *QJsonDocument::JsonFormat::Compact*

    */
    QString getHeaderQStr(const QJsonDocument::JsonFormat &format = QJsonDocument::JsonFormat::Indented) const;

    /**

    \brief Sets the JWT *header* from a QJsonDocument.
    \param jdocHeader JWT *header* as a QJsonDocument.
    \return true if the header was set, false if the header was not set.

    This method checks for a valid header format and returns false if the header is invalid.

    */
    bool setHeaderJDoc(const QJsonDocument &jdocHeader);

    /**

    \brief Sets the JWT *header* from a QString.
    \param jdocHeader JWT *header* as a QString.
    \return true if the header was set, false if the header was not set.

    This method checks for a valid header format and returns false if the header is invalid.

    */
    bool setHeaderQStr(const QString &strHeader);

    /**

    \brief Returns the JWT *payload* as a QJsonDocument.
    \return JWT *payload* as a QJsonDocument.

    */
    QJsonDocument getPayloadJDoc() const;

    /**

    \brief Returns the JWT *payload* as a QString.
    \param format Defines the format of the JSON returned.
    \return JWT *payload* as a QString.

    Format can be *QJsonDocument::JsonFormat::Indented* or *QJsonDocument::JsonFormat::Compact*

    */
    QString getPayloadQStr(const QJsonDocument::JsonFormat &format = QJsonDocument::JsonFormat::Indented) const;

    /**

    \brief Sets the JWT *payload* from a QJsonDocument.
    \param jdocHeader JWT *payload* as a QJsonDocument.
    \return true if the payload was set, false if the payload was not set.

    This method checks for a valid payload format and returns false if the payload is invalid.

    */
    bool setPayloadJDoc(const QJsonDocument &jdocPayload);

    /**

    \brief Sets the JWT *payload* from a QString.
    \param jdocHeader JWT *payload* as a QString.
    \return true if the payload was set, false if the payload was not set.

    This method checks for a valid payload format and returns false if the payload is invalid.

    */
    bool setPayloadQStr(const QString &strPayload);

    /**

    \brief Returns the JWT *algorithm* as a QString.
    \return JWT *algorithm* as a QString.

    */
    QString getAlgorithmStr() const;

    /**

    \brief Sets the JWT *algorithm* from a QString.
    \param strAlgorithm JWT *algorithm* as a QString.
    \return true if the algorithm was set, false if the algorithm was not set.

    This method checks for a valid supported algorithm. Valid values are:

    "HS256", "HS384" and "HS512".

    \sa QJsonWebToken::supportedAlgorithms().

    */
    bool setAlgorithmStr(const QString &strAlgorithm);

    QString getToken() const noexcept;

    /**

    \brief Sets the complete JWT as a QString.
    \param strToken Complete JWT as a QString.
    \return true if the complete JWT was set, false if not set.

    This method checks for a valid JWT format. It overwrites the *header*,
    *payload* , *signature* and *algorithm*. It does **not** overwrite the secret.

    */
    bool setToken(const QString &strToken);

    /**

    \brief Returns the current set of characters used to create random secrets.
    \return Set of characters as a QString.

    The default value is "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    \sa QJsonWebToken::setRandomSecret()
    \sa QJsonWebToken::setRandAlphanum()

    */

    /**

    \brief Checks validity of current JWT with respect to secret.
    \return true if the JWT is valid with respect to secret, else false.

    Uses the current *secret* to calculate a temporary *signature* and compares it to the
    current signature to check if they are the same. If they are, true is returned, if not then
    false is returned.

    */
    bool isValid() const;

    /**

    \brief Creates a QJsonWebToken instance from the complete JWT.
    \param strToken Complete JWT as a QString.
    \return Instance of QJsonWebToken.

    The JWT provided must have a valid format, else a QJsonWebToken instance with default
    values will be returned.

    */
    static QJsonWebToken fromToken(const QString &strToken);

    /**

    \brief Returns a list of the supported algorithms.
    \return List of supported algorithms as a QStringList.

    */
    static QStringList supportedAlgorithms();

    /**

    \brief Convenience method to append a claim to the *payload*.
    \param strClaimType The claim type as a QString.
    \param strValue The value type as a QString.

    Both parameters must be non-empty. If the claim type already exists, the current
    claim value is updated.

    */
    void appendClaim(const QString &strClaimType, const QString &strValue);

    /**

    \brief Convenience method to remove a claim from the *payload*.
    \param strClaimType The claim type as a QString.

    If the claim type does not exist in the *payload*, then this method does nothins.

    */
    void removeClaim(const QString &strClaimType);

    /**

    \brief Convenience method to return the data of a claim from the *payload* as a string.
    \param strClaimType The claim type as a QString.

    If the claim type does not exist in the *payload*, then this method returns a default
    constructed QString.

    */
    QString claim(const QString &strClaimType);

    qint64 getExp() const;

    QString getUserName() const;

private:
    bool isAlgorithmSupported(const QString &strAlgorithm);
    friend uint qHash(const QJsonWebToken &key, uint seed);

private:
    /// Properties
    QJsonDocument _header;	     // unencoded
    QJsonDocument _payload;      // unencoded
    QByteArray    _signature;    // unencoded
    QString       _algorithm;
    QString       _token;
};

#endif // QJSONWEBTOKEN_H
