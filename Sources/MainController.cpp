#include "MainController.hpp"
#include "FileManager.hpp"
#include "ReportReader.hpp"
#include "SolutionModel.hpp"
#include "ResultData.hpp"

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
    , m_filePendingDeletion{}
    , m_enumHelper{this}
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
        fillCDData(data);
        if (m_cdAlgo.setInputData(&m_cdData) != CDResult::Success) {
            askConfirm("Ошибка подготовки данных", "Что-то пошло не так");
            return Status::Fail;
        }
    } else if (m_currAlgorithm == AlgoType::GD) {
        fillGDData(data);
        if (m_gdAlgo.setInputData(&m_gdData) != GDResult::Success) {
            askConfirm("Ошибка подготовки данных", "Что-то пошло не так");
            return Status::Fail;
        }
    } else {
        askConfirm("Ошибка подготовки данных", "Алгоритм не поддерживается");
        return Status::Fail;
    }
    return Status::Success;
}

Status MainController::solve()
{
    if (m_currAlgorithm == AlgoType::CD) {
        if (m_cdAlgo.solve() != CDResult::Success) {
            askConfirm("Ошибка при решении", "Что-то пошло не так");
            return Status::Fail;
        }
    } else if (m_currAlgorithm == AlgoType::GD) {
        if (m_gdAlgo.solve() != GDResult::Success) {
            askConfirm("Ошибка при решении", "Что-то пошло не так");
            return Status::Fail;
        }
    } else {
        askConfirm("Ошибка при решении", "Алгоритм не поддерживается");
        return Status::Fail;
    }
    openReport(m_writer.fileName());
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

Status MainController::openReport(const QString &fileName)
{
    auto input = new InputData();
    auto model = new SolutionModel();
    auto result = new ResultData();
    QJsonArray solution;
    auto rv = ReportReader::reportData(fileName, input, solution, result);
    switch (rv) {
        case ReportStatus::NoResult:
            delete result;
            result = nullptr;
        case ReportStatus::Ok:
        case ReportStatus::InvalidCRC:
            break;
        default: {
            delete input;
            input = nullptr;
            delete model;
            model = nullptr;
            delete result;
            result = nullptr;
            askConfirm("Ошибка открытия файла", m_enumHelper.reportStatusToString(rv));
            return Status::Fail;
        }
    }
    model->setData(solution);
    auto report = new Report(fileName, input, model, result, this);
    input->setParent(report);
    model->setParent(report);
    if (result) result->setParent(report);
    m_openReports.append(report);
    emit openReportsUpdated();
    return Status::Success;
}


void MainController::closeReport(const QString &fileName)
{
    const size_t reportsCnt = m_openReports.count();
    for (size_t i = 0; i < reportsCnt; ++i) {
        if (m_openReports[i]->fileName() == fileName) {
            m_openReports.remove(i, 1);
            emit openReportsUpdated();
            return;
        }
    }
}

void MainController::requestDeleteReport(const QString &fileName)
{
    m_filePendingDeletion = fileName;
    QString text = "Файл " + fileName + " будет удален навсегда.";
    askConfirm("Удалить файл?", text, true);
    return;
}

void MainController::deleteConfirmed()
{
    closeReport(m_filePendingDeletion);
    m_quickInfoModel.deleteEntry(m_filePendingDeletion);
    FileManager::deleteFile(m_filePendingDeletion);
    askConfirm("Уведомление", "Файл " + m_filePendingDeletion + " успешно удалён");
}

Status MainController::inputDataFromFile(const QString &fileName, InputData *out)
{
    auto rv = ReportReader::inputData(fileName, out);
    if (rv == ReportStatus::Ok) {
        return Status::Success;
    }
    askConfirm("Ошибка открытия файла", m_enumHelper.reportStatusToString(rv));
    return Status::Fail;
}

void MainController::fillCDData(const InputData *data)
{
    return;
}

void MainController::fillGDData(const InputData *data)
{
    return;
}
