#include "MainController.hpp"

MainController::MainController(QObject *parent)
    : QObject{parent}
    , m_writer{}
    , m_currAlgorithm{AlgoType::ALL}
    , m_currExtension{ExtensionType::B}
    , m_cdAlgo{&m_writer}
    , m_cdData{}
    , m_gdAlgo{&m_writer}
    , m_gdData{}
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
