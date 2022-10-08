#include "client.h"

namespace Client
{
    /*
        QByteArray input = QByteArray::fromBase64( QString("eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzUxMiJ9").toUtf8() );
        qDebug() << input.size() << "IN:>" << input.toStdString().c_str();
        QJsonDocument jdocHeader = QJsonDocument::fromJson( input ) ;

        QJsonObject jobjHeader = jdocHeader.object();
        qDebug() << jobjHeader;

        QByteArray qsHeader64 =  jdocHeader.toJson(QJsonDocument::JsonFormat::Compact);//.toBase64() ;
        qDebug() << qsHeader64.size() << "OUT:>" << QString(qsHeader64);

        qDebug() << qsHeader64.toBase64();
        qDebug() << "------------does order matter-------";
        qDebug() << QByteArray("123456789").toBase64();
        qDebug() << QByteArray("987654321").toBase64();
     */
}
