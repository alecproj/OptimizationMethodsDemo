//
// Created on 25 Oct, 2025
//  by alecproj
//

#ifndef COORDINATEDESCENT_COMMON_HPP_
#define COORDINATEDESCENT_COMMON_HPP_

#include <string>
#include <vector>


// ============================================================================
// ������������ ��������� � ������
// ============================================================================

// ��������� ���������� ���������
enum class CDResult : int {
    Success            = 0,   // �������� ����������
    Fail              = -1,   // ����� ������
    InvalidInput      = -2,   // �������� ������� ������
    NoConvergence     = -3,   // ��� ����������
    OutOfBounds       = -4,   // ����� �� �������
    MaxIterations     = -5,   // ���������� ����. ����� ��������
    MaxFunctionsCalls = -6,   // ���������� ����. ����� ������� �������
    ParseError        = -7,   // ������ �������� �������
    ComputeError      = -8,   // �������������� ������
    NonDifferentiable = -9    // ������� �� ���. ����������������
};

// ��� ��������� �����������
enum class AlgorithmType {
    BASIC_COORDINATE_DESCENT,    // ������� �������� ��������������� ������
    STEEPEST_COORDINATE_DESCENT, // ����������: �������������� ����� ������� ������������� ������
    DERIVATIVE_FREE_DESCENT      // �������������� ����� (��� �����������)
};


// ��� ����������
enum class ExtremumType {
    MINIMUM, // �������
    MAXIMUM  // ��������
};

// ��� ���� � ���������
enum class StepType {
    CONSTANT,    // ���������� ���:     step = const;
    COEFFICIENT, // �������������� ���: step = k * �����������
    ADAPTIVE     // ���������� ���:     step ����������� ������������� �� ������ ��������
};

// ��������� ���������� ���������

enum class AlgorithmState {
    NOT_STARTED,  // �������� �� �������
    INITIALIZING, // ������������� ���������
    RUNNING ,      // �������� �����������
    CONVERGED,    // �������� ��������
    STOPPED,      // �������� �������� ������ �������
    FAILED        // �������� �������� ������ � �������
};

// ============================================================================
// ��������� ������� ������
// ============================================================================

// ��������� ��� ��������� �������� ��������������� ������
struct BasicAlgorithmParams {
    long double x_step  = 0.1;               // ��� �� X
    long double y_step = 0.1;                // ��� �� Y
    long double coefficient_step = 1.0;      // �������������� ���
    StepType step_type = StepType::CONSTANT; // ��� ����
    long double gradient_epsilon = 1e-8;     // �������� ���������� ���������
};

// ��������� ��� ��������� ��������������� ������ ������� ������������� ������
struct SteepestAlgorithmParams {

    long double learning_rate = 0.01;         // �������� "��������"
    long double gradient_epsilon = 1e-8;      // �������� ���������� ���������
    long double line_search_precision = 1e-6; // �������� ��������� ������

};

// ��������� ��� ��������������� ��������������� ������
struct DerivativeFreeAlgorithmParams {
    long double initial_step = 0.1;      // ��������� ���
    long double step_reduction = 0.5;    // ����������� ���������� ����
    long double exploration_step = 0.01; // ��� ��� ������������
    int max_exploration_attempts = 10;   // ����. ������� ������������
};

// �������� ������� ��������� ��� ���������
struct CoordinateInput {

    // --- ������������ ��������� ---
    std::string function;         // ������� ��� �����������
    AlgorithmType algorithm_type; // ��� ���������
    ExtremumType extremum_type;   // ��� ����������

    // --- ��������� ������� ---
    long double initial_x = 0.0; // ��������� ����������� X
    long double initial_y = 0.0; // ��������� ����������� Y

    // --- ������� ������ ---
    long double x_left_bound = -10.0; // ����� ������� ��������� X
    long double x_right_bound = 10.0; // ������ ������� ��������� X
    long double y_left_bound = -10.0; // ����� ������� ��������� Y
    long double y_right_bound = 10.0; // ������ ������� ��������� Y

    // --- ��������� �������� ---
    long double result_precision = 1e-06;     // �������� ����������
    long double computation_precision = 1e-8; // �������� ����������
    
    // --- ���������-����������� ��������� ---
    BasicAlgorithmParams basic_params;                    // ��� BASIC_COORDINATE_DESCENT
    SteepestAlgorithmParams steepest_params;              // ��� STEEPEST_COORDINATE_DESCENT
    DerivativeFreeAlgorithmParams derivative_free_params; // ��� DERIVATIVE_FREE_DESCENT      

    // --- ����������� ---
    int max_iterations = 1000;       // ����. ����� �������� 
    int max_function_calls = 10000;  // ����. ����� ������� �������

    // --- �������������� ����� ---
    bool enable_logging = false;         // ��������� �����������
    bool save_iteration_history = false; // ���������� ������� ��������

};

// ============================================================================
// ��������� �������� ������
// ============================================================================

// ��������� �����������
struct CoordinateResult {
    CDResult status = CDResult::Success;    // ������ ����������
    std::vector<long double> optimum_point; // ����� ���������� [X, Y]
    long double optimum_value;              // �������� ������� � ����� ����������
    int iterations = 0;                     // ���-�� ����������� ��������
    int function_calls = 0;                 // ���-�� ������� �������
    std::string message;                    // ��������� � ����������
};

// ============================================================================
// ��������������� �������
// ============================================================================

// ����������� ���������� ��������� � ��������� ���������
inline std::string resultToString(CDResult result) {
    switch (result) {
        case CDResult::Success:           return "Success";
        case CDResult::Fail:              return "General failure";
        case CDResult::InvalidInput:      return "Invalid input parameters";
        case CDResult::NoConvergence:     return "No convergence";
        case CDResult::OutOfBounds:       return "Point out of bounds";
        case CDResult::MaxIterations:     return "Maximum iterations exceeded";
        case CDResult::MaxFunctionsCalls: return "Maximum function calls exceeded";
        case CDResult::ParseError:        return "Function parsing error";
        case CDResult::ComputeError:      return "Computation error";
        case CDResult::NonDifferentiable: return "Function is not differentiable";
        
        default:                          return "Unknown result";
    }
}

// ����������� ���� ��������� � ��������� ���������
inline std::string algorithmTypeToString(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::BASIC_COORDINATE_DESCENT:    return "Basic Coordinate Descent";
        case AlgorithmType::STEEPEST_COORDINATE_DESCENT: return "Steepest Coordinate Descent";
        case AlgorithmType::DERIVATIVE_FREE_DESCENT:     return "Derivative-Free Coordinate Descent";
        
        default:                                         return "Unknown Algorithm";
    }
}

// ����������� ���� ���������� � ������
inline std::string extremumTypeToString(ExtremumType type) {
    switch (type) {
        case ExtremumType::MINIMUM: return "Minimum";
        case ExtremumType::MAXIMUM: return "Maximum";

        default:                    return "Unknow";
    }
}

// ��������: ��������� �� ��������� ����������� �������
inline bool algorithmRequiresDerivatives(AlgorithmType type) {
    return type == AlgorithmType::BASIC_COORDINATE_DESCENT ||
        type == AlgorithmType::STEEPEST_COORDINATE_DESCENT;
}

#endif // COORDINATEDESCENT_COMMON_HPP_