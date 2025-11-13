//
// Created on 3 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_APPENUMS_HPP_
#define SOURCES_APPENUMS_HPP_

#include <QObject>
#include <cassert>

class ReportStatus: public QObject {
    Q_OBJECT
public:
    enum Status {
        NoSolution          = -8,
        NoResult            = -7,
        CheckFailed         = -6,
        InvalidDataStruct   = -5,
        InvalidCRC          = -4,
        InvalidFile         = -3,
        InvalidName         = -2,
        FileDoesNotExists   = -1,
        Ok                  = 0,
        NotVerified         = 1
    };
    Q_ENUM(Status)

    explicit ReportStatus(QObject *parent = nullptr) : QObject(parent) {}
};

class Result: public QObject {
    Q_OBJECT
public:
    enum Type {
        Fail        = -1,
        Success     = 0,
    };
    Q_ENUM(Type)

    explicit Result(QObject *parent = nullptr) : QObject(parent) {}
};

class StepType : public QObject {
    Q_OBJECT
public:
    enum Type {
        CONSTANT    = 0,
        COEFFICIENT = 1,
        ADAPTIVE    = 2
    };
    Q_ENUM(Type)

    explicit StepType(QObject *parent = nullptr) : QObject(parent) {}
};

class ExtremumType : public QObject {
    Q_OBJECT
public:
    enum Type {
        MINIMUM  = 0,
        MAXIMUM  = 1
    };
    Q_ENUM(Type)

    explicit ExtremumType(QObject *parent = nullptr) : QObject(parent) {}
};

class AlgoType : public QObject {
    Q_OBJECT
public:
    enum Type {
        ALL = 0,
        CD  = 1, // Coordinate Descent
        GD  = 2  // Gradient Descent
    };
    Q_ENUM(Type)

    explicit AlgoType(QObject *parent = nullptr) : QObject(parent) {}
};

class ExtensionType : public QObject {
    Q_OBJECT
public:
    enum Type {
        B   = 0, // Basic
        S  = 1,  // Steepest descent method
        R  = 2   // Ravine method
    };
    Q_ENUM(Type)

    explicit ExtensionType(QObject *parent = nullptr) : QObject(parent) {}
};

class FullAlgoType : public QObject {
    Q_OBJECT
public:
    enum Type {
        INVALID = 0,
        CDB     = 1, // Coordinate Descent Basic
        CDS     = 2, // Coordinate Descent Steepest
        GDB     = 3, // Gradient Descent Basic
        GDS     = 4, // Gradient Descent Steepest
        GDR     = 5  // Gradient Descent Ravine
    };
    Q_ENUM(Type)

    explicit FullAlgoType(QObject *parent = nullptr) : QObject(parent) {}
};

class CheckList : public QObject {
    Q_OBJECT
public:
    enum Check { 
        Error               = -1,
        CalcAccuracy        = (1 << 0),  // CDB,CDS,GDB,GDS,GDR
        ResultAccuracy      = (1 << 1),  // CDB,CDS,GDB,GDS,GDR
        StartX1             = (1 << 2),  // CDB,CDS,GDB,GDS,GDR 
        StartY1             = (1 << 3),  // CDB,CDS,GDB,GDS,GDR 
        StartX2             = (1 << 4),  //    ,   ,   ,   ,
        StartY2             = (1 << 5),  //                ,
        StepX               = (1 << 6),  // CDB,CDS,   ,   ,
        StepY               = (1 << 7),  // CDB,CDS,   ,   ,
        Step                = (1 << 8),  //    ,   ,GDB,   ,
        MinX                = (1 << 9),  // CDB,CDS,GDB,GDS,GDR
        MaxX                = (1 << 10), // CDB,CDS,GDB,GDS,GDR
        MinY                = (1 << 11), // CDB,CDS,GDB,GDS,GDR
        MaxY                = (1 << 12), // CDB,CDS,GDB,GDS,GDR
        Iterations          = (1 << 13), // CDB,CDS,GDB,GDS,GDR 
        FuncCalls           = (1 << 14), // CDB,CDS,GDB,GDS,GDR

        CDBCheck            = 32463,
        CDSCheck            = 32463,
        GDBCheck            = 32527,
        GDSCheck            = 32271,
        GDRCheck            = 32271
    };
    Q_ENUM(Check)

    explicit CheckList(QObject *parent = nullptr) : QObject(parent) {
        static_assert(CDBCheck == (
            CalcAccuracy + ResultAccuracy + StartX1 + StartY1 +
            StepX        + StepY          + MinX    + MaxX    + 
            MinY         + MaxY           + Iterations + FuncCalls
        ), "Invalid CDBCheck");
        static_assert(CDSCheck == (
            CalcAccuracy + ResultAccuracy + StartX1 + StartY1 +
            StepX + StepY + MinX + MaxX + MinY + MaxY +
            Iterations + FuncCalls
        ), "Invalid CDSCheck");
        static_assert(GDBCheck == (
            CalcAccuracy    + ResultAccuracy + StartX1 + StartY1 +
            Step + MinX           + MaxX    + MinY    +
            MaxY            + Iterations     + FuncCalls
        ), "Invalid GDBCheck");
        static_assert(GDSCheck == (
            CalcAccuracy + ResultAccuracy + StartX1 + StartY1 +
            MinX         + MaxX           + MinY    + MaxY    +
            Iterations   + FuncCalls
        ), "Invalid GDSCheck");
        static_assert(GDRCheck == (
            CalcAccuracy + ResultAccuracy + StartX1 + StartY1 +
             + MinX  + MaxX + MinY + MaxY + Iterations + FuncCalls
        ), "Invalid GDRCheck");
    }

};

class EnumHelper : public QObject {
    Q_OBJECT
public:

    explicit EnumHelper(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE FullAlgoType::Type calculateFullType(AlgoType::Type algo, ExtensionType::Type extension)
    {
        if (algo == AlgoType::CD) {
            if (extension == ExtensionType::B) {
                return FullAlgoType::CDB;
            } else if (extension == ExtensionType::S) {
                return FullAlgoType::CDS;
            }
        } else if (algo == AlgoType::GD) {
            if (extension == ExtensionType::B) {
                return FullAlgoType::GDB;
            } else if (extension == ExtensionType::S) {
                return FullAlgoType::GDS;
            } else if (extension == ExtensionType::R) {
                return FullAlgoType::GDR;
            }
        }
        return FullAlgoType::INVALID;
    }

    Q_INVOKABLE CheckList::Check getCheckByFullType(FullAlgoType::Type type) 
    {
        if (type == FullAlgoType::CDB) {
            return CheckList::CDBCheck;
        } else if (type == FullAlgoType::CDS) {
            return CheckList::CDSCheck;
        } else if (type == FullAlgoType::GDB) {
            return CheckList::GDBCheck;
        } else if (type == FullAlgoType::GDS) {
            return CheckList::GDSCheck;
        } else if (type == FullAlgoType::GDR) {
            return CheckList::GDRCheck;
        }
        return CheckList::Error;
    }

    Q_INVOKABLE QString algoTypeToString(AlgoType::Type type)
    {
        switch (type) {
            case AlgoType::CD:
                return "Метод покоординатного спуска";
            case AlgoType::GD:
                return "Метод градиентного спуска";
            default:
                return "";
        }
    }

    Q_INVOKABLE QString extensionTypeToString(ExtensionType::Type type)
    {
        switch (type) {
            case ExtensionType::B:
                return "Базовый";
            case ExtensionType::S:
                return "Метод наискорейшего спуска";
            case ExtensionType::R:
                return "Овражный метод";
            default:
                return "";
        }
    }

    Q_INVOKABLE QString stepTypeToString(StepType::Type type)
    {
        switch (type) {
            case StepType::CONSTANT:
                return "Константный";
            case StepType::COEFFICIENT:
                return "Коэффициентный";
            case StepType::ADAPTIVE:
                return "Адаптивный";
            default:
                return "";
        }
    }

    Q_INVOKABLE QString extremumTypeToString(ExtremumType::Type type)
    {
        switch (type) {
            case ExtremumType::MINIMUM:
                return "Минимум";
            case ExtremumType::MAXIMUM:
                return "Максимум";
            default:
                return "";
        }
    }

    Q_INVOKABLE QString reportStatusToString(ReportStatus::Status status)
    {
        switch (status) {
            case ReportStatus::NoSolution:
                return "Отчет не содержит вычислений";
            case ReportStatus::NoResult:
                return "Отчет не содержит результат";
            case ReportStatus::CheckFailed:
                return "При проверке файла что-то пошло не так";
            case ReportStatus::InvalidDataStruct:
                return "Неверная структура файла (не хватает данных)";
            case ReportStatus::InvalidCRC:
                return "Файл был изменен или поврежден";
            case ReportStatus::InvalidFile:
                return "Файл не является отчетом";
            case ReportStatus::InvalidName:
                return "Неверное имя файла";
            case ReportStatus::FileDoesNotExists:
                return "Файл не существует";
            case ReportStatus::Ok:
                return "Файл корректен";
            case ReportStatus::NotVerified:
                return "Проверка файла не выполнена";
            default:
                return "";
        }
    }
};


#endif // SOURCES_APPENUMS_HPP_
