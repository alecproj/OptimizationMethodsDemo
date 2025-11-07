#include "MainController.hpp"
#include "FileManager.hpp"
#include "ReportReader.hpp"

MainController::MainController(QObject *parent)
    : QObject{parent}
    , m_writer{}
    , m_currAlgorithm{AlgoType::ALL}
    , m_currExtension{ExtensionType::B}
    , m_cdAlgo{&m_writer}
    , m_cdData{}
    , m_gdAlgo{&m_writer}
    , m_gdData{}
    , m_quickInfoModel{this}
    , m_openReports{}
{
}

Status MainController::setInputData(const InputData *data)
{
    if (!data) {
        return Status::Fail;
    }
    m_writer.setInputData(data);
    m_currAlgorithm = data->algorithmId();
    m_currExtension = data->extensionId();
    if (m_currAlgorithm == AlgoType::CD) {
        if (m_cdAlgo.setInputData(&m_cdData) != CDResult::Success) {
            return Status::Fail;
        }
    } else if (m_currAlgorithm == AlgoType::GD) {
        if (m_gdAlgo.setInputData(&m_gdData) != GDResult::Success) {
            return Status::Fail;
        }
    } else {
        return Status::Fail;
    }
    return Status::Success;
}

Status MainController::solve()
{
    if (m_currAlgorithm == AlgoType::CD) {
        if (m_cdAlgo.solve() != CDResult::Success) {
            return Status::Fail;
        }
    } else if (m_currAlgorithm == AlgoType::GD) {
        if (m_gdAlgo.solve() != GDResult::Success) {
            return Status::Fail;
        }
    } else {
        return Status::Fail;
    }

    return Status::Success;
}

void MainController::updateQuickInfoModel()
{
    bool updates = false;
    QStringList files = FileManager::listFiles();
    for (const auto file : files) {
        if (!m_quickInfoModel.exists(file)) {
            auto info = new QuickInfo(&m_quickInfoModel);
            auto rv = ReportReader::quickInfo(file, info);
            if (rv == ReportStatus::Ok) {
                m_quickInfoModel.prepend(info);
                updates = true;
            } else {
                delete info;
            }
        }
    }
    if (updates) {
        emit quickInfoModelChanged();
    }
}

void MainController::openReport(const QString &fileName)
{
    ReportData data;
    ReportReader::reportData(fileName, &data);
    auto model = new ReportModel(this);
    model->setData(data.solution, fileName);
    m_openReports.append(model);
    emit openReportsUpdated();
}

Status MainController::inputDataFromFile(const QString &fileName, InputData *out)
{
    auto rv = ReportReader::inputData(fileName, out);
    if (rv == ReportStatus::Ok || rv == ReportStatus::InvalidCRC) {
        return Status::Success;
    }
    return Status::Fail;
}

