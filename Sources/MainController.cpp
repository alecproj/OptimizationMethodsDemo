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
        auto rv = m_cdAlgo.setInputData(&m_cdData);
        qDebug() << "SET INPUT" << static_cast<int>(rv);
        if (rv != CD::Result::Success) {
            askConfirm(
                "Ошибка подготовки данных",
                QString::fromStdString(CD::resultToString(rv))
            );
            return Status::Fail;
        }
    } else if (m_currAlgorithm == AlgoType::GD) {
        fillGDData(data);
        auto rv = m_gdAlgo.setInputData(&m_gdData);
        if (rv != GD::Result::Success) {
            askConfirm(
                "Ошибка подготовки данных",
                QString::fromStdString(GD::resultToString(rv))
            );
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
        auto rv = m_cdAlgo.solve();
        qDebug() << "ALGO RESUULT: " << static_cast<int>(rv);
        if (rv != CD::Result::Success) {
            askConfirm(
                "Ошибка при решении",
                QString::fromStdString(CD::resultToString(rv))
            );
            return Status::Fail;
        }
    } else if (m_currAlgorithm == AlgoType::GD) {
        auto rv = m_gdAlgo.solve();
        if (rv != GD::Result::Success) {
            askConfirm(
                "Ошибка при решении",
                QString::fromStdString(GD::resultToString(rv))
            );
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
    m_cdData.function = data->function().toStdString();
    m_cdData.algorithm_type = static_cast<CD::AlgorithmType>(data->extensionId());
    m_cdData.extremum_type = static_cast<CD::ExtremumType>(data->extremumId());
    m_cdData.step_type = static_cast<CD::StepType>(data->stepId());
    m_cdData.step_type_x = m_cdData.step_type;
    m_cdData.step_type_y = m_cdData.step_type;
    m_cdData.initial_x = data->startX1();
    m_cdData.initial_y = data->startY1();
    m_cdData.x_left_bound = data->minX();
    m_cdData.x_right_bound = data->maxX();
    m_cdData.y_left_bound = data->minY();
    m_cdData.y_right_bound = data->maxY();
    if (m_cdData.step_type == CD::StepType::COEFFICIENT) {
        m_cdData.coefficient_step_size_x = data->stepX();
        m_cdData.coefficient_step_size_y = data->stepY();
    } else {
        m_cdData.constant_step_size_x = data->stepX();
        m_cdData.constant_step_size_y = data->stepY();
    }
    m_cdData.result_precision = data->resultAccuracy();
    m_cdData.computation_precision = data->calcAccuracy();
    m_cdData.max_iterations = data->maxIterations();
    m_cdData.max_function_calls = data->maxFuncCalls();
}

void MainController::fillGDData(const InputData *data)
{
    m_gdData.function = data->function().toStdString();
    m_gdData.algorithm_type = static_cast<GD::AlgorithmType>(data->extensionId());
    m_gdData.extremum_type = static_cast<GD::ExtremumType>(data->extremumId());

    m_gdData.initial_x = data->startX1();
    m_gdData.initial_y = data->startY1();
    m_gdData.x_left_bound = data->minX();
    m_gdData.x_right_bound = data->maxX();
    m_gdData.y_left_bound = data->minY();
    m_gdData.y_right_bound = data->maxY();
    m_gdData.constant_step_size = data->step();
    m_gdData.coefficient_step_size = data->step();
    m_gdData.result_precision = data->resultAccuracy();
    m_gdData.computation_precision = data->calcAccuracy();
    m_gdData.max_iterations = data->maxIterations();
    m_gdData.max_function_calls = data->maxFuncCalls();

    m_gdData.step_type = static_cast<GD::StepType>(data->stepId());
    if(m_gdData.step_type == GD::StepType::CONSTANT || m_gdData.step_type == GD::StepType::ADAPTIVE){
        m_gdData.constant_step_size = data->step();
    } else if(m_gdData.step_type == GD::StepType::COEFFICIENT){
        m_gdData.coefficient_step_size = data->step();
    }

}
