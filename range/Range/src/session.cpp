/*********************************************************************
 *  AUTHOR: Tomas Soltys                                             *
 *  FILE:   session.cpp                                              *
 *  GROUP:  Range                                                    *
 *  TYPE:   source file (*.cpp)                                      *
 *  DATE:   17-th January 2012                                       *
 *                                                                   *
 *  DESCRIPTION: Session class definition                            *
 *********************************************************************/

#include <QDesktopServices>

#include "session.h"
#include "main_settings.h"
#include "model_io.h"
#include "job_manager.h"

Session::Session()
{
    RLogger::trace("Session::Session()\n");
    this->sessionID = QDateTime::currentDateTimeUtc().toString("yyyyMMddHHmmsszzz");
    this->downloadManager = new DownloadManager(this);
    this->drawEngine = new DrawEngine(this);

    QObject::connect(&this->pickList,&PickList::pickListChanged,this,&Session::onPickListChanged);
    QObject::connect(this,&Session::modelAdded,this,&Session::onModelAdded);
    QObject::connect(this,&Session::modelChanged,this,&Session::onModelChanged);
    QObject::connect(MainSettings::getInstance().getApplicationSettings(),
                     &ApplicationSettings::nHistoryRecordsChanged,
                     this,
                     &Session::onNHistoryRecordsChanged);
}

Session & Session::getInstance(void)
{
    // No trace - too many messages.
    static Session session;
    return session;
}

const QString &Session::getID(void) const
{
    return this->sessionID;
}

void Session::lock(void)
{
    RLogger::trace("void Session::lock(void)\n");
    this->mutex.lock();
}

bool Session::trylock(int timeout)
{
    RLogger::trace("bool Session::trylock(int timeout)\n");
    return this->mutex.tryLock(timeout);
}

void Session::unlock(void)
{
    RLogger::trace("void Session::unlock(void)\n");
    this->mutex.unlock();
}

DownloadManager *Session::getDownloadManager(void)
{
    RLogger::trace("DownloadManager *Session::getDownloadManager(void)\n");
    return this->downloadManager;
}

DrawEngine *Session::getDrawEngine(void)
{
    RLogger::trace("DrawEngine *Session::getDrawEngine(void)\n");
    return this->drawEngine;
}

const QString &Session::getFileName(void) const
{
    RLogger::trace("const QString &Session::getFileName(void) const\n");
    return this->fileName;
}

uint Session::getNModels(void) const
{
    RLogger::trace("uint Session::getNModels(void) const\n");
    return (uint)this->models.size();
}

const Model & Session::getModel(uint position) const
{
//    RLogger::trace("const Model & Session::getModel(uint position) const\n");
    return this->models[position];
}

Model & Session::getModel(uint position)
{
//    RLogger::trace("Model & Session::getModel(uint position)\n");
    return this->models[position];
}

const Model * Session::getModelPtr(uint position) const
{
    RLogger::trace("const Model * Session::getModelPtr(uint position) const\n");
    return &this->models[position];
}

Model * Session::getModelPtr(uint position)
{
    RLogger::trace("Model * Session::getModelPtr(uint position)\n");
    return &this->models[position];
}

void Session::addModel(const Model &model)
{
    RLogger::trace("void Session::addModel(const Model &model)\n");
    bool locked = this->trylock();
    this->models.push_back(model);
    this->models.last().initializeMeshInput();
    if (locked)
    {
        this->unlock();
    }
    emit this->modelAdded(this->models.size()-1);
}

void Session::setModel(uint position, const Model &model)
{
    RLogger::trace("void Session::setModel(uint position, const Model &model)\n");
    bool locked = this->trylock();
    this->models[position] = model;
    if (locked)
    {
        this->unlock();
    }
    this->setModelChanged(position);
}

void Session::removeModel(uint position)
{
    RLogger::trace("void Session::removeModel(uint position)\n");
    bool locked = this->trylock();
    this->models.removeAt(int(position));
    this->pickList.removeItems(position);
    if (locked)
    {
        this->unlock();
    }
    emit this->modelRemoved(position);
}

void Session::setModelSelected(uint modelID, bool selected)
{
    RLogger::trace("void Session::setModelSelected(uint modelID, bool selected)\n");
    for (uint i=0;i<this->getModel(modelID).getNPoints();i++)
    {
        this->setEntitySelected(modelID,R_ENTITY_GROUP_POINT,i,selected,false);
    }
    for (uint i=0;i<this->getModel(modelID).getNLines();i++)
    {
        this->setEntitySelected(modelID,R_ENTITY_GROUP_LINE,i,selected,false);
    }
    for (uint i=0;i<this->getModel(modelID).getNSurfaces();i++)
    {
        this->setEntitySelected(modelID,R_ENTITY_GROUP_SURFACE,i,selected,false);
    }
    for (uint i=0;i<this->getModel(modelID).getNVolumes();i++)
    {
        this->setEntitySelected(modelID,R_ENTITY_GROUP_VOLUME,i,selected,false);
    }
    for (uint i=0;i<this->getModel(modelID).getNVectorFields();i++)
    {
        this->setEntitySelected(modelID,R_ENTITY_GROUP_VECTOR_FIELD,i,selected,false);
    }
    for (uint i=0;i<this->getModel(modelID).getNScalarFields();i++)
    {
        this->setEntitySelected(modelID,R_ENTITY_GROUP_SCALAR_FIELD,i,selected,false);
    }
    for (uint i=0;i<this->getModel(modelID).getNStreamLines();i++)
    {
        this->setEntitySelected(modelID,R_ENTITY_GROUP_STREAM_LINE,i,selected,false);
    }
    for (uint i=0;i<this->getModel(modelID).getNCuts();i++)
    {
        this->setEntitySelected(modelID,R_ENTITY_GROUP_CUT,i,selected,false);
    }
    for (uint i=0;i<this->getModel(modelID).getNIsos();i++)
    {
        this->setEntitySelected(modelID,R_ENTITY_GROUP_ISO,i,selected,false);
    }
    this->getModel(modelID).setSelected(selected);
    this->setModelSelectionChanged(modelID);
}

void Session::setEntitySelected(uint modelID, REntityGroupType elementGrpType, uint entityID, bool selected, bool notifyModel)
{
    RLogger::trace("void Session::setEntitySelected(uint modelID, REntityGroupType elementGrpType, uint entityID, bool selected, bool notifyModel)\n");
    if (this->getModel(modelID).getSelected(elementGrpType,entityID) != selected)
    {
        this->getModel(modelID).setSelected(elementGrpType,entityID,selected);
        emit this->entitySelectionChanged(modelID,elementGrpType,entityID,selected);
        if (notifyModel)
        {
            this->setModelSelectionChanged(modelID);
        }
    }
}

void Session::setEntityVisible(uint modelID, REntityGroupType elementGrpType, uint entityID, bool visible)
{
    RLogger::trace("void Session::setEntityVisible(uint modelID, REntityGroupType elementGrpType, uint entityID, bool visible)\n");
    if (this->getModel(modelID).getVisible(elementGrpType,entityID) != visible)
    {
        this->getModel(modelID).setVisible(elementGrpType,entityID,visible);
        emit this->entityVisibilityChanged(modelID,elementGrpType,entityID,visible);
        emit this->modelVisibilityChanged(modelID);
    }
}

void Session::setModelVisible(uint modelID, bool visible)
{
    RLogger::trace("void Session::setModelVisible(uint modelID, bool visible)\n");
    for (uint i=0;i<this->getModel(modelID).getNPoints();i++)
    {
        this->setEntityVisible(modelID,R_ENTITY_GROUP_POINT,i,visible);
    }
    for (uint i=0;i<this->getModel(modelID).getNLines();i++)
    {
        this->setEntityVisible(modelID,R_ENTITY_GROUP_LINE,i,visible);
    }
    for (uint i=0;i<this->getModel(modelID).getNSurfaces();i++)
    {
        this->setEntityVisible(modelID,R_ENTITY_GROUP_SURFACE,i,visible);
    }
    for (uint i=0;i<this->getModel(modelID).getNVolumes();i++)
    {
        this->setEntityVisible(modelID,R_ENTITY_GROUP_VOLUME,i,visible);
    }
    for (uint i=0;i<this->getModel(modelID).getNVectorFields();i++)
    {
        this->setEntityVisible(modelID,R_ENTITY_GROUP_VECTOR_FIELD,i,visible);
    }
    for (uint i=0;i<this->getModel(modelID).getNScalarFields();i++)
    {
        this->setEntityVisible(modelID,R_ENTITY_GROUP_SCALAR_FIELD,i,visible);
    }
    for (uint i=0;i<this->getModel(modelID).getNStreamLines();i++)
    {
        this->setEntityVisible(modelID,R_ENTITY_GROUP_STREAM_LINE,i,visible);
    }
    for (uint i=0;i<this->getModel(modelID).getNCuts();i++)
    {
        this->setEntityVisible(modelID,R_ENTITY_GROUP_CUT,i,visible);
    }
    for (uint i=0;i<this->getModel(modelID).getNIsos();i++)
    {
        this->setEntityVisible(modelID,R_ENTITY_GROUP_ISO,i,visible);
    }
}

QList<uint> Session::getSelectedModelIDs(void) const
{
    RLogger::trace("QList<uint> Session::getSelectedModelIDs(void) const\n");
    QList<uint> selectedModelIDs;

    for (uint i=0;i<this->getNModels();i++)
    {
        // Check if model is partialy selected (NOT completely unselected)
        if (!this->getModel(i).isSelected(false))
        {
            selectedModelIDs.push_back(i);
        }
    }

    return selectedModelIDs;
}

QList<uint> Session::getVisibleModelIDs(void) const
{
    RLogger::trace("QList<uint> Session::getVisibleModelIDs(void) const\n");
    QList<uint> visibleModelIDs;

    for (uint i=0;i<this->getNModels();i++)
    {
        // Check if model is partialy visible (NOT completely unselected)
        if (!this->getModel(i).isVisible(false))
        {
            visibleModelIDs.push_back(i);
        }
    }

    return visibleModelIDs;
}

bool Session::isModelSelected(uint modelID)
{
//    RLogger::trace("bool Session::isModelSelected(uint modelID)\n");
    return (!this->getModel(modelID).isSelected(false));
}

QList<SessionEntityID> Session::getAllEntityIDs(void) const
{
    RLogger::trace("QList<SessionEntityID> Session::getAllEntityIDs(void) const\n");
    QList<SessionEntityID> allEntityIDs;

    for (uint i=0;i<this->getNModels();i++)
    {
        allEntityIDs += this->getAllEntityIDs(i);
    }
    return allEntityIDs;
}

QList<SessionEntityID> Session::getAllEntityIDs(uint modelID) const
{
    RLogger::trace("QList<SessionEntityID> Session::getAllEntityIDs(uint modelID) const\n");
    QList<SessionEntityID> allEntityIDs;
    SessionEntityID entityID;

    entityID.setMid(modelID);

    entityID.setType(R_ENTITY_GROUP_POINT);
    for (uint j=0;j<this->getModel(modelID).getNPoints();j++)
    {
        entityID.setEid(j);
        allEntityIDs.push_back(entityID);
    }

    entityID.setType(R_ENTITY_GROUP_LINE);
    for (uint j=0;j<this->getModel(modelID).getNLines();j++)
    {
        entityID.setEid(j);
        allEntityIDs.push_back(entityID);
    }

    entityID.setType(R_ENTITY_GROUP_SURFACE);
    for (uint j=0;j<this->getModel(modelID).getNSurfaces();j++)
    {
        entityID.setEid(j);
        allEntityIDs.push_back(entityID);
    }

    entityID.setType(R_ENTITY_GROUP_VOLUME);
    for (uint j=0;j<this->getModel(modelID).getNVolumes();j++)
    {
        entityID.setEid(j);
        allEntityIDs.push_back(entityID);
    }

    entityID.setType(R_ENTITY_GROUP_VECTOR_FIELD);
    for (uint j=0;j<this->getModel(modelID).getNVectorFields();j++)
    {
        entityID.setEid(j);
        allEntityIDs.push_back(entityID);
    }

    entityID.setType(R_ENTITY_GROUP_SCALAR_FIELD);
    for (uint j=0;j<this->getModel(modelID).getNScalarFields();j++)
    {
        entityID.setEid(j);
        allEntityIDs.push_back(entityID);
    }

    entityID.setType(R_ENTITY_GROUP_STREAM_LINE);
    for (uint j=0;j<this->getModel(modelID).getNStreamLines();j++)
    {
        entityID.setEid(j);
        allEntityIDs.push_back(entityID);
    }

    entityID.setType(R_ENTITY_GROUP_CUT);
    for (uint j=0;j<this->getModel(modelID).getNCuts();j++)
    {
        entityID.setEid(j);
        allEntityIDs.push_back(entityID);
    }

    entityID.setType(R_ENTITY_GROUP_ISO);
    for (uint j=0;j<this->getModel(modelID).getNIsos();j++)
    {
        entityID.setEid(j);
        allEntityIDs.push_back(entityID);
    }

    return allEntityIDs;
}

QList<SessionEntityID> Session::getSelectedEntityIDs(void) const
{
    RLogger::trace("QList<SessionEntityID> Session::getSelectedEntityIDs(void) const\n");
    QList<SessionEntityID> selectedEntityIDs;

    for (uint i=0;i<this->getNModels();i++)
    {
        selectedEntityIDs += this->getModel(i).getSelectedEntityIDs(i);
    }
    return selectedEntityIDs;
}

QList<SessionEntityID> Session::getVisibleEntityIDs(void) const
{
    RLogger::trace("QList<SessionEntityID> Session::getVisibleEntityIDs(void) const\n");
    QList<SessionEntityID> visibleEntityIDs;

    for (uint i=0;i<this->getNModels();i++)
    {
        visibleEntityIDs += this->getModel(i).getVisibleEntityIDs(i);
    }
    return visibleEntityIDs;
}

QList<SessionEntityID> Session::filterSelectedEntityIDs(const QList<SessionEntityID> selectedEntityIDs, uint modelID, REntityGroupTypeMask entityTypeMask)
{
    RLogger::trace("QList<SessionEntityID> Session::filterSelectedEntityIDs(const QList<SessionEntityID> selectedEntityIDs, uint modelID, REntityGroupTypeMask entityTypeMask)\n");
    QList<SessionEntityID> selectedEntities(selectedEntityIDs);

    for (int i=selectedEntities.size()-1;i>=0;i--)
    {
        if (selectedEntities[i].getMid() == modelID
            &&
            selectedEntities[i].getType() & entityTypeMask
            &&
            R_ENTITY_GROUP_TYPE_IS_ELEMENT(selectedEntities[i].getType()))
        {
            continue;
        }
        selectedEntities.erase(selectedEntities.begin()+i);
    }

    return selectedEntities;
}

uint Session::findModelByName(const QString &modelName)
{
    RLogger::trace("uint Session::findModelByName(const QString &modelName)\n");
    for (uint i=0;i<this->getNModels();i++)
    {
        if (this->getModel(i).getName() == modelName)
        {
            return i;
        }
    }
    return RConstants::eod;
}

uint Session::findModelByPtr(const Model *pModel)
{
    RLogger::trace("uint Session::findModelByPtr(const Model *pModel)\n");
    for (uint i=0;i<this->getNModels();i++)
    {
        if (this->getModelPtr(i) == pModel)
        {
            return i;
        }
    }
    return RConstants::eod;
}

void Session::setModelSelectionChanged(uint modelID)
{
    RLogger::trace("void Session::setModelSelectionChanged(uint modelID)\n");
    emit this->modelSelectionChanged(modelID);
}

void Session::setDisplayPropertiesChanged(uint modelID, REntityGroupType elementGrpType, uint entityID)
{
    RLogger::trace("void Session::setDisplayPropertiesChanged(uint modelID, REntityGroupType elementGrpType, uint entityID)\n");
    emit this->displayPropertiesChanged(modelID,elementGrpType,entityID);
}

void Session::setModelChanged(uint modelID)
{
    RLogger::trace("void Session::setModelChanged(uint modelID)\n");
    emit this->modelChanged(modelID);
}

void Session::setModelRenamed(uint modelID)
{
    RLogger::trace("void Session::setModelRenamed(uint modelID)\n");
    emit this->modelRenamed(modelID);
}

void Session::setProblemChanged(uint modelID)
{
    RLogger::trace("void Session::setProblemChanged(uint modelID)\n");
    emit this->problemChanged(modelID);
}

void Session::setProblemSelectionChanged(uint modelID)
{
    RLogger::trace("void Session::setProblemSelectionChanged(uint modelID)\n");
    emit this->problemSelectionChanged(modelID);
}

void Session::setResultsChanged(uint modelID)
{
    RLogger::trace("void Session::setResultsChanged(uint modelID)\n");
    emit this->resultsChanged(modelID);
}

void Session::setBoundaryConditionChanged(uint modelID, REntityGroupType elementGrpType, uint entityID)
{
    RLogger::trace("void Session::setBoundaryConditionChanged(uint modelID, REntityGroupType elementGrpType, uint entityID)\n");
    emit this->boundaryConditionChanged(modelID,elementGrpType,entityID);
}

void Session::setInitialConditionChanged(uint modelID, REntityGroupType elementGrpType, uint entityID)
{
    RLogger::trace("void Session::setInitialConditionChanged(uint modelID, REntityGroupType elementGrpType, uint entityID)\n");
    emit this->initialConditionChanged(modelID,elementGrpType,entityID);
}

void Session::setEnvironmentConditionChanged(uint modelID, REntityGroupType elementGrpType, uint entityID)
{
    RLogger::trace("void Session::setEnvironmentConditionChanged(uint modelID, REntityGroupType elementGrpType, uint entityID)\n");
    emit this->environmentConditionChanged(modelID,elementGrpType,entityID);
}

void Session::setMaterialChanged(uint modelID, REntityGroupType elementGrpType, uint entityID)
{
    RLogger::trace("void Session::setMaterialChanged(uint modelID, REntityGroupType elementGrpType, uint entityID)\n");
    emit this->materialChanged(modelID,elementGrpType,entityID);
}

void Session::setVariableDataChanged(uint modelID, RVariableType variableType)
{
    RLogger::trace("void Session::setVariableDataChanged(uint modelID, RVariableType variableType)\n");
    emit this->variableDataChanged(modelID,variableType);
}

void Session::setVariableDataChanged(const SessionEntityID &entityID, RVariableType variableType)
{
    RLogger::trace("void Session::setVariableDataChanged(const SessionEntityID &entityID, RVariableType variableType)\n");
    emit this->variableDataChanged(entityID,variableType);
}

const PickList &Session::getPickList(void) const
{
    RLogger::trace("const PickList &Session::getPickList(void) const\n");
    return this->pickList;
}

PickList &Session::getPickList(void)
{
    RLogger::trace("PickList &Session::getPickList(void)\n");
    return this->pickList;
}

void Session::setBeginDrawStreamLinePosition(const RR3Vector &position)
{
    RLogger::trace("void Session::setBeginDrawStreamLinePosition(const RR3Vector &position)\n");
    emit this->beginDrawStreamLinePosition(position);
}

void Session::setEndDrawStreamLinePosition(void)
{
    RLogger::trace("void Session::setEndDrawStreamLinePosition(void)\n");
    emit this->endDrawStreamLinePosition();
}

void Session::setBeginDrawScaleOrigin(const RR3Vector &position)
{
    RLogger::trace("void Session::setBeginDrawScaleOrigin(const RR3Vector &position)\n");
    emit this->beginDrawScaleOrigin(position);
}

void Session::setEndDrawScaleOrigin(void)
{
    RLogger::trace("void Session::setEndDrawScaleOrigin(void)\n");
    emit this->endDrawScaleOrigin();
}

void Session::setBeginDrawRotationOrigin(const RR3Vector &position)
{
    RLogger::trace("void Session::setBeginDrawRotationOrigin(const RR3Vector &position)\n");
    emit this->beginDrawRotationOrigin(position);
}

void Session::setEndDrawRotationOrigin(void)
{
    RLogger::trace("void Session::setEndDrawRotationOrigin(void)\n");
    emit this->endDrawRotationOrigin();
}

void Session::setBeginDrawCutPlane(const RPlane &plane)
{
    RLogger::trace("void Session::setBeginDrawCutPlane(const RPlane &plane)\n");
    emit this->beginDrawCutPlane(plane);
}

void Session::setEndDrawCutPlane(void)
{
    RLogger::trace("void Session::setEndDrawCutPlane(void)\n");
    emit this->endDrawCutPlane();
}

void Session::setBeginDrawMoveNodes(const QMap<SessionNodeID, RR3Vector> &nodesToMove)
{
    RLogger::trace("void Session::setBeginDrawMoveNodes(const QMap<SessionNodeID, RR3Vector> &nodesToMove)\n");
    emit this->beginDrawMoveNodes(nodesToMove);
}

void Session::setEndDrawMoveNodes(void)
{
    RLogger::trace("void Session::setEndDrawMoveNodes(void)\n");
    emit this->endDrawMoveNodes();
}

void Session::setTakeScreenShot(uint modelID, const QString &screenShotFileName, bool blocking)
{
    RLogger::trace("void Session::setTakeScreenShot(uint modelID, const QString &screenShotFileName, bool blocking)\n");
    if (blocking)
    {
        emit this->takeScreenShotBlocking(modelID, screenShotFileName);
    }
    else
    {
        emit this->takeScreenShot(modelID, screenShotFileName);
    }
}

void Session::readModels(const QStringList &fileNames)
{
    RLogger::info("Loading %u files\n",fileNames.size());
    RLogger::indent();
    for (int i=0;i<fileNames.size();i++)
    {
        RLogger::info("Loading %u: \'%s\'\n",i+1,fileNames.at(i).toUtf8().constData());

        if (!fileNames.at(i).isEmpty())
        {
            // Read model.
            ModelIO *modelIO = new ModelIO(MODEL_IO_OPEN, fileNames.at(i));
            modelIO->setAutoDelete(true);

            JobManager::getInstance().submit(modelIO);
        }
    }
    RLogger::unindent();
}

void Session::read(const QString &fileName)
{
    RLogger::trace("void Session::read(const QString &fileName)\n");
    if (fileName.isEmpty())
    {
        throw RError(R_ERROR_INVALID_FILE_NAME,R_ERROR_REF,"No file name was provided.");
    }

    RLogger::info("Reading session file \'%s\'.\n",fileName.toUtf8().constData());
    RLogger::indent();

    this->fileName = fileName;

    RFile sessionFile(this->fileName,RFile::ASCII);

    if (!sessionFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        RLogger::unindent();
        throw RError(R_ERROR_OPEN_FILE,R_ERROR_REF,"Failed to open the file \'%s\'.",this->fileName.toUtf8().constData());
    }

    RLogger::info("Reading session file records.\n");
    RLogger::indent();

    uint nModels = 0;
    RFileIO::readAscii(sessionFile,nModels);
    RLogger::info("Detected %u file records.\n",nModels);
    QStringList modelFileNames;
    for (uint i=0;i<nModels;i++)
    {
        QString modelFileName;
        RFileIO::readAscii(sessionFile,modelFileName);

        if (modelFileName.isEmpty())
        {
            RLogger::info("File record %u is empty.\n",i+1);
        }
        else
        {
            RLogger::info("File record %u \'%s\'\n",i+1,modelFileName.toUtf8().constData());
            modelFileNames.append(modelFileName);
        }
    }
    RLogger::unindent();

    sessionFile.close();

    this->readModels(modelFileNames);

    RLogger::unindent();
}

void Session::write(const QString &fileName, bool writeModels)
{
    RLogger::trace("void Session::write(const QString &fileName, bool writeModels)\n");
    RLogger::info("Writing session file \'%s\'.\n",fileName.toUtf8().constData());

    if (fileName.isEmpty())
    {
        throw RError(R_ERROR_INVALID_FILE_NAME,R_ERROR_REF,"No file name was provided.");
    }

    this->fileName = fileName;

    RSaveFile sessionFile(this->fileName,RSaveFile::ASCII);

    if (!sessionFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        throw RError(R_ERROR_OPEN_FILE,R_ERROR_REF,"Failed to open the file \'%s\'.",this->fileName.toUtf8().constData());
    }

    RFileIO::writeAscii(sessionFile,this->getNModels(),true);
    for (uint i=0;i<this->getNModels();i++)
    {
        RFileIO::writeAscii(sessionFile,"\"" + this->getModel(i).getFileName() + "\"",true);

        if (writeModels)
        {
            if (this->getModel(i).getFileName().isEmpty())
            {
                QDir dataDir(MainSettings::getInstancePtr()->getDataDir());
                QString binaryExtension = RModel::getDefaultFileExtension(true);
                QString modelName = this->getModel(i).getName();
                if (modelName.length() == 0)
                {
                    modelName = "Model_" + QString::number(i+1);
                }
                QString modelFileName = dataDir.filePath(modelName + "." + binaryExtension);
                this->getModel(i).setFileName(modelFileName);
            }

            // Write model.
            ModelIO *modelIO = new ModelIO(MODEL_IO_SAVE, this->getModel(i).getFileName(), this->getModelPtr(i));
            modelIO->setAutoDelete(true);

            JobManager::getInstance().submit(modelIO);
        }
    }

    sessionFile.commit();
}

void Session::clear(void)
{
    RLogger::trace("void Session::clear(void)\n");
    RLogger::info("Closing session\n");
    this->fileName.clear();

    while (this->getNModels() > 0)
    {
        RLogger::indent();
        RLogger::info("Closing model \'%s\'\n",this->getModel(this->getNModels()-1).getName().toUtf8().constData());

        Session::getInstance().removeModel(this->getNModels()-1);

        RLogger::unindent();
    }
}

void Session::storeCurentModelVersion(uint modelID, const QString &message)
{
    RLogger::trace("void Session::storeCurentModelVersion(uint modelID, const QString &message)\n");
    this->getModel(modelID).storeCurentVersion(MainSettings::getInstance().getApplicationSettings()->getNHistoryRecords(),message);
}

bool Session::isUndoAvailable(void) const
{
    RLogger::trace("bool Session::isUndoAvailable(void)\n");
    QList<uint> selectedModelIDs = this->getSelectedModelIDs();
    for (int i=0;i<selectedModelIDs.size();i++)
    {
        if (this->getModel(selectedModelIDs[i]).getUndoStackSize() > 0)
        {
            return true;
        }
    }
    return false;
}

bool Session::isRedoAvailable(void) const
{
    RLogger::trace("bool Session::isRedoAvailable(void)\n");
    QList<uint> selectedModelIDs = this->getSelectedModelIDs();
    for (int i=0;i<selectedModelIDs.size();i++)
    {
        if (this->getModel(selectedModelIDs[i]).getRedoStackSize() > 0)
        {
            return true;
        }
    }
    return false;
}

QString Session::getUndoTooltip(void) const
{
    RLogger::trace("QString Session::getUndoTooltip(void)\n");
    QString undoMessage;

    QList<uint> selectedModelIDs = this->getSelectedModelIDs();
    for (int i=0;i<selectedModelIDs.size();i++)
    {
        if (!undoMessage.isEmpty())
        {
            undoMessage += "; ";
        }
        undoMessage += this->getModel(selectedModelIDs[i]).getUndoActionMessage();
    }
    undoMessage = tr("Undo") + (undoMessage.isEmpty() ? QString() : ": " + undoMessage);
    return undoMessage;
}

QString Session::getRedoTooltip(void) const
{
    RLogger::trace("QString Session::getRedoTooltip(void)\n");
    QString redoMessage;

    QList<uint> selectedModelIDs = this->getSelectedModelIDs();
    for (int i=0;i<selectedModelIDs.size();i++)
    {
        if (!redoMessage.isEmpty())
        {
            redoMessage += "; ";
        }
        redoMessage += this->getModel(selectedModelIDs[i]).getRedoActionMessage();
    }
    redoMessage = tr("Redo") + (redoMessage.isEmpty() ? QString() : ": " + redoMessage);
    return redoMessage;
}

QString Session::getDefaultFileExtension(void)
{
    RLogger::trace("QString Session::getDefaultFileExtension(void)\n");
    return "ras";
}

QString Session::getDefaultFileName(void)
{
    RLogger::trace("QString Session::getDefaultFileName(void)\n");
    QDir sesionDir = MainSettings::getInstancePtr()->getSessionDir();
    return sesionDir.filePath(QString("default.") + Session::getDefaultFileExtension());
}

bool Session::selectedModelsHasEntities(REntityGroupTypeMask entityTypeMask)
{
    RLogger::trace("bool Session::selectedModelsHasEntities(REntityGroupTypeMask entityTypeMask)\n");
    QList<uint> modelIDs = Session::getInstance().getSelectedModelIDs();

    for (int i=0;i<modelIDs.size();i++)
    {
        if (((entityTypeMask & R_ENTITY_GROUP_POINT) && Session::getInstance().getModel(modelIDs[i]).getNPoints())
            ||
            ((entityTypeMask & R_ENTITY_GROUP_LINE) && Session::getInstance().getModel(modelIDs[i]).getNLines())
            ||
            ((entityTypeMask & R_ENTITY_GROUP_SURFACE) && Session::getInstance().getModel(modelIDs[i]).getNSurfaces())
            ||
            ((entityTypeMask & R_ENTITY_GROUP_VOLUME) && Session::getInstance().getModel(modelIDs[i]).getNVolumes())
            ||
            ((entityTypeMask & R_ENTITY_GROUP_CUT) && Session::getInstance().getModel(modelIDs[i]).getNCuts())
            ||
            ((entityTypeMask & R_ENTITY_GROUP_ISO) && Session::getInstance().getModel(modelIDs[i]).getNIsos())
            ||
            ((entityTypeMask & R_ENTITY_GROUP_STREAM_LINE) && Session::getInstance().getModel(modelIDs[i]).getNStreamLines())
            ||
            ((entityTypeMask & R_ENTITY_GROUP_SCALAR_FIELD) && Session::getInstance().getModel(modelIDs[i]).getNScalarFields())
            ||
            ((entityTypeMask & R_ENTITY_GROUP_VECTOR_FIELD) && Session::getInstance().getModel(modelIDs[i]).getNVectorFields()))
        {
            return true;
        }
    }
    return false;
}

bool Session::selectedModelsHasEntitySelected(REntityGroupType entityGroupType)
{
    RLogger::trace("bool Session::selectedModelsHasEntitySelected(REntityGroupType entityGroupType)\n");
    QList<uint> modelIDs = Session::getInstance().getSelectedModelIDs();

    for (int i=0;i<modelIDs.size();i++)
    {
        foreach (SessionEntityID sessionID, Session::getInstance().getModel(modelIDs[i]).getSelectedEntityIDs(modelIDs[i]))
        {
            if (sessionID.getType() == entityGroupType)
            {
                return true;
            }
        }
    }
    return false;
}

void Session::onPickListChanged(void)
{
    RLogger::trace("void Session::onPickListChanged(void)\n");
    emit this->pickListChanged();
}

void Session::onModelAdded(uint modelID)
{
    RLogger::trace("void Session::onModelAdded(uint modelID)\n");
    this->getModel(modelID).createDependentEntities();
}

void Session::onModelChanged(uint modelID)
{
    RLogger::trace("void Session::onModelChanged(uint modelID)\n");
    this->getModel(modelID).createDependentEntities();
}

void Session::onNHistoryRecordsChanged(uint nHistoryRecords)
{
    RLogger::trace("void Session::onNHistoryRecordsChanged(uint nHistoryRecords)\n");
    for (uint i=0;i<this->getNModels();i++)
    {
        this->getModel(i).updateHistoryStackSize(nHistoryRecords);
    }
    emit this->nHistoryRecordsChanged(nHistoryRecords);
}

void Session::onSoftwareUpdateFileSaved(uint downloadID, const QString &fileName)
{
    RLogger::trace("void Session::onSoftwareUpdateFileSaved(uint downloadID, const QString &fileName)\n");
    RLogger::info("Opening downloaded file \'%s\' (id=\'%u\')\n",fileName.toUtf8().constData(),downloadID);

    if (!QDesktopServices::openUrl(QUrl("file:///" + fileName)))
    {
        RLogger::error("Failed to open the file \'%s\'\n",fileName.toUtf8().constData());
    }
}
