#include "MainController.hpp"
#include "FileManager.hpp"
#include "ReportReader.hpp"
#include "SolutionModel.hpp"
#include "ResultData.hpp"

MainController::MainController(QObject *parent)
    : QObject{parent}
    , m_writer{}
    , m_currPartition{PartType::NONE}
    , m_currAlgorithm{AlgoType::ALL}
    , m_currExtension{LO::ExtensionType::B}
    , m_cdAlgo{&m_writer}
    , m_cdData{}
    , m_gdAlgo{&m_writer}
    , m_gdData{}
    , m_cgAlgo{&m_writer}
    , m_cgData{}
    , m_quickInfoModel{this}
    , m_openReports{this}
    , m_filePendingDeletion{}
    , m_enumHelper{this}
{
}

Status MainController::setInputData(const InputData *data)
{
    if (!data) {
        return Status::InvalidPointer;
    }

    m_currPartition = data->partitionId();
    switch (m_currPartition) {
        case PartType::LO:
            return setLOInputData(dynamic_cast<const LO::InputData *>(data));
        case PartType::GO:
            return setGOInputData(dynamic_cast<const GO::InputData *>(data));
        default:
            return Status::InvalidPartType;
    }
}

Status MainController::solve()
{
    if (m_currPartition == PartType::LO) {
        return solveLO();
    } else if (m_currPartition == PartType::GO) {
        return solveGO();
    } else {
        qCritical() << "Unknown partition type when solving";
        return Status::InvalidPartType;
    }
}

Status MainController::solveLO()
{
    if (m_currAlgorithm == AlgoType::CD) {
        auto rv = m_cdAlgo.solve();
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
    } else if (m_currAlgorithm == AlgoType::CG) {
        auto rv = m_cgAlgo.solve();
        if (rv != CG::Result::Success) {
            askConfirm(
                "Ошибка при решении",
                QString::fromStdString(CG::resultToString(rv))
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

Status MainController::solveGO()
{
    qWarning() << "Solving GO..." ;
    return Status::Fail;
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
    InputData *input;
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
    auto report = new Report(input->partitionId(),
        fileName, input, model, result, this);
    input->setParent(report);
    model->setParent(report);
    if (result) result->setParent(report);
    m_openReports.appendReport(report);
    emit openReportsUpdated();
    return Status::Success;
}


void MainController::closeReport(const QString &fileName)
{
    m_openReports.removeReport(fileName);
    emit openReportsUpdated();
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

Status MainController::setLOInputData(const LO::InputData *data)
{
    if (!data) {
        return Status::InvalidPointer;
    }
    m_writer.setInputData(data);
    m_currAlgorithm = data->algorithmId();
    m_currExtension = data->extensionId();
    if (m_currAlgorithm == AlgoType::CD) {
        fillCDData(data);
        auto rv = m_cdAlgo.setInputData(&m_cdData);
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
    } else if (m_currAlgorithm == AlgoType::CG) {
        fillCGData(data);
        auto rv = m_cgAlgo.setInputData(&m_cgData);
        if (rv != CG::Result::Success) {
            askConfirm(
                "Ошибка подготовки данных",
                QString::fromStdString(CG::resultToString(rv))
            );
            return Status::Fail;
        }
    } else {
        askConfirm("Ошибка подготовки данных", "Алгоритм не поддерживается");
        return Status::InvalidAlgoType;
    }
    return Status::Success;

}

Status MainController::setGOInputData(const GO::InputData *data)
{
    qDebug() << "Setting up the input data for the GO...";
    if (!data) {
        return Status::InvalidPointer;
    }
    m_writer.setInputData(data);
    return Status::Fail;
}

void MainController::fillCDData(const LO::InputData *data)
{
    qDebug() << "Setting up the input data for the LO CoordinateDescent algorithm...";
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

void MainController::fillGDData(const LO::InputData *data)
{
    qDebug() << "Setting up the input data for the LO GradientDescent algorithm...";
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

void MainController::fillCGData(const LO::InputData *data)
{
    qDebug() << "Setting up the input data for the LO ConjugateGradient algorithm...";
    m_cgData.function = data->function().toStdString();
    m_cgData.algorithm_type = static_cast<CG::AlgorithmType>(data->extensionId());
    m_cgData.extremum_type = static_cast<CG::ExtremumType>(data->extremumId());

    m_cgData.initial_x = data->startX1();
    m_cgData.initial_y = data->startY1();
    m_cgData.x_left_bound = data->minX();
    m_cgData.x_right_bound = data->maxX();
    m_cgData.y_left_bound = data->minY();
    m_cgData.y_right_bound = data->maxY();
    m_cgData.result_precision = data->resultAccuracy();
    m_cgData.computation_precision = data->calcAccuracy();
    m_cgData.max_iterations = data->maxIterations();
    m_cgData.max_function_calls = data->maxFuncCalls();
}
