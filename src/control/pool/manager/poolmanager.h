#pragma once

#include "control/pool/poolentity.h"
#include "exception/illegalargumentexception.h"
#include "sessiondata.h"

/**
 * @brief The PoolManager class
 * Provides the management of the VLSI design process.
 * In order to get manager working, you should call enable().
 */
class PoolManager : public PoolEntity
{
    Q_OBJECT

public:
    /**
     * @brief PoolManager
     * Constructs the object.
     * @param selfPort - the port the transmitter will be set to.
     */
    PoolManager(Version programVersion, int selfPort = 0);
    ~PoolManager();

    void enable();
    void disable();

    void connectToUnconnectedNodes();
    void connectToNode(PoolEntityInfo& info);

    void removeNode(PoolEntityInfo& info);

    void start();
    bool isStarted() const { return started; }

    QList<PoolEntityInfo>& getPoolNodesInfo() { return connectedEntities; }

    SessionData* getSessionData() const { return data; }
    void setSessionData(SessionData* data);

signals:
    void sendClearNodesInfo();
    void sendUpdateNodeInfo(PoolEntityInfo& info);
    void sendRemoveNodeInfo(int index);

    void sendLog(QString log, LogType type = LogType::Common);
    void sendError(QString error);

    void sendDisconnected(QHostAddress address, int tcpPort);

private slots:
    void onNewConnection(QHostAddress address, int tcpPort);
    void onDisconnected(QHostAddress address, int tcpPort);
    void onDataReceived(QByteArray* data, QHostAddress, int);

    void onSendVersion(QUuid uuid, Version version);

protected:
    void connectDispatcher();

private:
    bool tryConnect(PoolEntityInfo& info);

    void disconnectFromNode(PoolEntityInfo& info);
    void disconnectFromNodeWithoutNotification(PoolEntityInfo& info);

    void setStatusOfAllConnectedNodes(NodeStatus status);

    static const EntityType entityType = EntityType::Manager;

    SessionData* data;
    bool started;

    Version programVersion;    
};
