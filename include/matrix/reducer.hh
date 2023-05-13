#include <type_traits> 
#include <stdint.h> 

namespace matrix {
    // 矩阵描述符
    class MatrixDescriptor {
        public: 
            // 矩阵行数 
            uint64_t row ; 
            // 矩阵列数 
            uint64_t col ; 
        public: 
            // 乘法；可空匹配 
            constexpr MatrixDescriptor operator * ( const MatrixDescriptor & other ) const noexcept {
                MatrixDescriptor out; 
                out.row = row; 
                out.col = other.col; 
                return out ;  
            } 
            constexpr bool multi_match( const MatrixDescriptor & other ) const noexcept {
                return col == 0 || other.row == 0 || col == other.row ; 
            } 
            constexpr bool multi_strict_match( const MatrixDescriptor & other ) const noexcept {
                return col != 0 && col == other.row ; 
            } 
            // 加法；可空匹配
            constexpr MatrixDescriptor operator + ( const MatrixDescriptor & other ) const noexcept {
                MatrixDescriptor out; 
                out.row = row == 0 ? other.row : row ; 
                out.col = col == 0 ? other.col : col ; 
                return out ; 
            } 
            constexpr bool match( const MatrixDescriptor & other ) const noexcept {
                return 
                    true 
                    && (row == 0 || other.row == 0 || row == other.row) 
                    && (col == 0 || other.col == 0 || col == other.col) ; 
            } 
            constexpr bool strict_match ( const MatrixDescriptor & other ) const noexcept {
                return row != 0 && col != 0 && row == other.row && col == other.col ; 
            } 
            bool operator == ( const MatrixDescriptor & other ) = delete ; 
    } ;
}

namespace matrix::reducer {
    // 矩阵运算算子：输入单元 & 归约单元 
    // 输入单元标志
    class InputUnit {
        public: 
            // 虚函数，启用多态 
            virtual ~InputUnit() = default ; 
    } ; 
    // 归约单元标志 
    class ReduceUnit {
        public: 
            // 虚函数，启用多态 
            virtual ~ReduceUnit() = default ; 
    } ; 
    // 矩阵运算算子约束 
    template <typename T> 
    concept Operator = ( std::is_base_of_v<T, InputUnit> ^ std::is_base_of_v<T, ReduceUnit> ) ; 
    
    // 矩阵属性描述符 
    template <MatrixDescriptor desc_z> 
    class MatrixDescriptor { 
        public: 
            // 矩阵描述符 
            static constexpr MatrixDescriptor des = desc_z ;
        public: 
            // 虚函数，启用多态 
            virtual ~MatrixDescriptor() = default ; 
    }; 
    
    // 矩阵加法约束描述符
    template <MatrixDescriptor lhs, MatrixDescriptor rhs> 
    requires (lhs.match(rhs)) 
    class AddOperator : public MatrixDescriptor< lhs + rhs > {
    }; 
    
    // 矩阵加法算子 
    template <typename Lhs, typename Rhs> 
    class AddMatrixDescriptor : public ReduceUnit , public AddOperator<Lhs::des, Rhs::des> {
        using LhsType = Lhs ; 
        using RhsType = Rhs ; 
    }; 
    
    template <MatrixDescriptor lhs, MatrixDescriptor rhs> 
    requires (lhs.multi_match(rhs))
    class MultiOperator : public MatrixDescriptor< lhs * rhs > {
    }; 
    
    // 矩阵乘法算子 
    template <typename Lhs, typename Rhs> 
    class MultiMatrixDescriptor : public ReduceUnit , public MultiOperator<Lhs::des, Rhs::des> {
        using LhsType = Lhs ; 
        using RhsType = Rhs ; 
    }; 
    
    template <Operator Lhs, Operator Rhs> 
    MultiMatrixDescriptor<Lhs, Rhs> operator * ( const Lhs & lhs, const Rhs & rhs ) noexcept {
        return MultiMatrixDescriptor<Lhs, Rhs>{} ; 
    } 
    
    template <Operator Lhs, Operator Rhs> 
    AddMatrixDescriptor<Lhs, Rhs> operator + ( const Lhs & lhs, const Rhs & rhs ) noexcept {
        return AddMatrixDescriptor<Lhs, Rhs>{} ; 
    } 
    
    class ArbitraryMatrixDescriptor : public MatrixDescriptor<matrix::MatrixDescriptor{0, 0}> , public InputUnit {
    }; 
    
    template <uint64_t col> 
    class ArbitraryRowMatrixDescriptor : public MatrixDescriptor<matrix::MatrixDescriptor{0, col}> , public InputUnit {
    }; 
    
    template <uint64_t row> 
    class ArbitraryColMatrixDescriptor : public MatrixDescriptor<matrix::MatrixDescriptor{row, 0}> , public InputUnit {
    }; 
    
    template <uint64_t row, uint64_t col> 
    class FixedMatrixDescriptor : public MatrixDescriptor<matrix::MatrixDescriptor{row, col}> , public InputUnit {
    }; 

}