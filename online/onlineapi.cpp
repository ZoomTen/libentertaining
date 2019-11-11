/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "onlineapi.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "onlinewebsocket.h"
#include "private/entertainingsettings.h"

struct OnlineApiPrivate {
    OnlineApi* instance = nullptr;
    QSettings* settings = EntertainingSettings::instance();
};

OnlineApiPrivate* OnlineApi::d = new OnlineApiPrivate();

OnlineApi*OnlineApi::instance()
{
    if (d->instance == nullptr) d->instance = new OnlineApi();
    return d->instance;
}

tPromise<QJsonDocument>*OnlineApi::post(QString endpoint, QJsonObject body)
{
    return new tPromise<QJsonDocument>([=](std::function<void(QJsonDocument)> res, std::function<void(QString)> rej) {
        QNetworkAccessManager* mgr = new QNetworkAccessManager();

        QUrl url("http://" + serverHost());
        url.setScheme(isServerSecure() ? "https" : "http");
        url.setPath("/api/" + endpoint);

        QNetworkRequest req(url);
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QString auth = authorizationHeader();
        if (!auth.isEmpty()) {
            req.setRawHeader("Authorization", auth.toUtf8());
        }

        QNetworkReply* reply = mgr->post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));
        connect(reply, &QNetworkReply::finished, this, [=] {
            processReply(reply, res, rej);
            mgr->deleteLater();
        });
    });
}

tPromise<QJsonDocument>*OnlineApi::get(QString endpoint)
{
    return new tPromise<QJsonDocument>([=](std::function<void(QJsonDocument)> res, std::function<void(QString)> rej) {
        QNetworkAccessManager* mgr = new QNetworkAccessManager();

        QUrl url("http://" + serverHost());
        url.setScheme(isServerSecure() ? "https" : "http");
        url.setPath("/api/" + endpoint);

        QNetworkRequest req(url);
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QString auth = authorizationHeader();
        if (!auth.isEmpty()) {
            req.setRawHeader("Authorization", auth.toUtf8());
        }

        QNetworkReply* reply = mgr->get(req);
        connect(reply, &QNetworkReply::finished, this, [=] {
            processReply(reply, res, rej);
            mgr->deleteLater();
        });
    });
}

tPromise<OnlineWebSocket*>*OnlineApi::play(QString applicationName, QString applicationVersion, QWidget*parentWidget)
{
    OnlineWebSocket* ws = new OnlineWebSocket(applicationName, applicationVersion, parentWidget);

    qRegisterMetaType<QAbstractSocket::SocketState>();

    QUrl url("ws://" + serverHost());
    url.setScheme(isServerSecure() ? "wss" : "ws");
    url.setPath("/api/play");

    if (d->settings->contains("online/token")) {
        QTimer::singleShot(500, this, [=] {
            ws->open(url);
        });
    }

    return new tPromise<OnlineWebSocket*>([=](std::function<void(OnlineWebSocket*)> res, std::function<void(QString)> rej) {
        if (!d->settings->contains("online/token")) {
            rej("!" + tr("You are not logged in."));
            return;
        }
        QMetaObject::Connection* disconnectedConnection = new QMetaObject::Connection();
        *disconnectedConnection = connect(ws, &OnlineWebSocket::disconnected, this, [=] {
            QString error;
            switch (static_cast<int>(ws->closeCode())) {
                case QWebSocketProtocol::CloseCodeNormal:
                    break;
                case QWebSocketProtocol::CloseCodeGoingAway:
                    error = tr("The connection was lost because the server is now undergoing maintenance.");
                    break;
                case QWebSocketProtocol::CloseCodeProtocolError:
                case QWebSocketProtocol::CloseCodeMissingExtension:
                case 1014: //Bad Gateway
                    error = tr("The connection was lost because a server error occurred.");
                    break;
                case QWebSocketProtocol::CloseCodeDatatypeNotSupported:
                case QWebSocketProtocol::CloseCodeWrongDatatype:
                    error = tr("The connection was lost because the server couldn't understand our messages.");
                    break;
                case QWebSocketProtocol::CloseCodeTooMuchData:
                    error = tr("The connection was lost because too much data was sent.");
                    break;
                case QWebSocketProtocol::CloseCodeBadOperation:
                    error = tr("The connection was lost because an internal error occurred.");
                    break;

                    //Application specific close codes
                case 4000: //Authentication Error
                    error = tr("The connection was lost because your credentials have expired or are incorrect.\n\nTry connecting again and re-enter your credentials.");

                    //Log the user out because this token is no longer valid
                    d->settings->remove("online/token");
                    break;
                case 4001: //Unknown Application
                    error = tr("The connection was lost because the server does not support this application.");
                    break;
                case 4002: //Bad Version
                    error = tr("The connection was lost because an update is required to continue playing online.");
                    break;

                case QWebSocketProtocol::CloseCodeAbnormalDisconnection:
                    //maybe come up with a specific message for this later
                default:
                    error = tr("The connection to the server was lost.");
            }

            rej("!" + error);
            ws->deleteLater();
        });

        QMetaObject::Connection* errorConnection = new QMetaObject::Connection();
        *errorConnection = connect(ws, QOverload<QAbstractSocket::SocketError>::of(&OnlineWebSocket::error), this, [=] {
            QString error;
            switch (ws->error()) {
                case QAbstractSocket::ConnectionRefusedError:
                    error = tr("The server refused the connection. It may be undergoing maintenance.");
                    break;
                case QAbstractSocket::HostNotFoundError:
                    error = tr("The DNS server was unable to locate the IP address of the Entertaining Games server.");
                    break;
                case QAbstractSocket::SocketTimeoutError:
                    error = tr("The connection to the Entertaining Games server timed out.\n\nTry again later.");
                    break;
                default:
                    error = ws->errorString();
                    break;
            }

            rej("!" + error);
            ws->deleteLater();
        });

        connect(ws, &OnlineWebSocket::ready, this, [=] {
            disconnect(*disconnectedConnection);
            delete disconnectedConnection;
            disconnect(*errorConnection);
            delete errorConnection;


            res(ws);
        });
    });
}

int OnlineApi::httpStatusCodeFromPromiseRejection(QString rejection)
{
    QStringList parts = rejection.split(":|");
    return parts.at(1).toInt();
}

QString OnlineApi::errorFromPromiseRejection(QString rejection)
{
    if (rejection.startsWith("!")) return rejection.mid(1);
    QStringList parts = rejection.split(":|");
    switch (httpStatusCodeFromPromiseRejection(rejection)) {
        case 400:
        case 500:
            return tr("The server is not operating correctly. Try again at a later time.");
        case 401:
            return tr("You are not authorized to perform this action now.");
        case 404:
            return tr("The server may not support this game because the requested path was not found.");
        default:
            return parts.at(3);
    }
}

OnlineApi::OnlineApi(QObject *parent) : QObject(parent)
{

}

QString OnlineApi::serverHost()
{
    QByteArray server = qgetenv("ENTERTAINING_ONLINE_HOST");
    if (server.isEmpty()) server = DEFAULT_ENTERTAINING_ONLINE_HOST;
    return server;
}

bool OnlineApi::isServerSecure()
{
    QByteArray isSecure = qgetenv("ENTERTAINING_ONLINE_HOST_IS_SECURE");
    bool secure;
    if (isSecure.isEmpty()) {
        secure = DEFAULT_ENTERTAINING_ONLINE_HOST_IS_SECURE;
    } else {
        secure = isSecure == "true";
    }
    return secure;
}

QString OnlineApi::authorizationHeader()
{
    if (d->settings->contains("online/token")) {
        return "Token " + d->settings->value("online/token").toString();
    } else {
        return "";
    }
}

QString OnlineApi::buildRejection(QNetworkReply*reply)
{
    QStringList list;
    list.append(QString::number(reply->error()));
    list.append(QString::number(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()));
    list.append(reply->errorString());
    return list.join(":|");
}

void OnlineApi::processReply(QNetworkReply* reply, std::function<void (QJsonDocument)> res, std::function<void (QString)> rej)
{
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 204) {
        //Respond with an empty document
        res(QJsonDocument());
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isNull()) {
            if (reply->error() == QNetworkReply::NoError) {
                rej("!" + tr("The server is not operating correctly."));
            } else {
                rej(OnlineApi::buildRejection(reply));
            }
        } else {
            res(doc);
        }
    }

    reply->deleteLater();
}