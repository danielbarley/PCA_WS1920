#include <iostream>
#include <iomanip>
#include <chrono>
#include <valarray>

// Classes
class Vector {
    public:
    std::valarray<double> data;
    Vector(unsigned int dim, bool random = false): data(dim) {
        if (random) {
            for (auto &x: data) {
                x = rand() %100;
            }
        }
    }

    double operator* (std::valarray<double> v) {
        double o = 0;
        for (size_t i = 0; i < v.size(); i++) {
            o += v[i] * data[i];
        }
        return o;
    }

    double& operator[] (unsigned int i) {
        return this->data[i];
    }
};

class Matrix {
    size_t nrow;
    size_t ncol;
    std::valarray<double> data;
    public:
        Matrix(unsigned int num_col, unsigned int num_row, bool random = false)
        : nrow(num_row),
          ncol(num_col),
          data(num_col * num_row){
              if (random) {
                  for (auto &x: data) {
                      x = rand() / rand();
                  }
              }
        };

        Matrix(unsigned int dim, bool random = false): Matrix(dim, dim, random) {};

        double operator() (unsigned int col, unsigned int row) const {
            return this->data[col*ncol+row];
        };
        double& operator() (unsigned int col, unsigned int row) {
            return this->data[col*nrow+row];
        };

        // accessing row
        std::valarray<double> operator() (unsigned int col) const {
            return data[std::slice(col*nrow, nrow, 1)];
        }
        std::slice_array<double> operator() (unsigned int col) {
            return data[std::slice(col*nrow, nrow, 1)];
        }

        // accessing col
        std::valarray<double> operator[] (unsigned int row) const {
            return data[std::slice(row, ncol, nrow)];
        }
        std::slice_array<double> operator[] (unsigned int row) {
            return data[std::slice(row, ncol, nrow)];
        }

        friend std::ostream &operator<< (std::ostream &output, const Matrix &M){
            for (size_t c =0; c < M.nrow; c++) {
                output << "| ";
                output << std::setprecision(4);
                for (size_t i = 0; i < M.ncol; i++)
                    output << M(i,c) << "\t";
                output << "|" << std::endl;
            }
            return output;
        }

        //variant 1: row-wise access
        Vector operator* (Vector v) {
            Vector o = Vector(this->ncol);
            for (size_t i = 0; i < this->ncol; i++) {
                o[i] = v * (std::valarray<double>)((*this)(i));
            }
            return o;
        }

        //Variant 2: column-wise access
        Vector operator| (Vector v) {
            Vector o = Vector(this->ncol);
            for (size_t i = 0; i < this->ncol; i++) {
                o[i] = v * (std::valarray<double>)((*this)[i]);
            }
            return o;
        }
};

int main() {
    srand(time(NULL));
    std::array<int, 6> sizes = {10, 100, 500, 1000, 5000, 10000};
    //std::array<int, 4> sizes = {10, 100, 500, 1000};

    for (auto size: sizes){

        Matrix A = Matrix(size, true);
        Vector x = Vector(size, true);

        auto row_start = std::chrono::high_resolution_clock::now();
        Vector r = A * x;
        auto row_end = std::chrono::high_resolution_clock::now();

        auto col_start = std::chrono::high_resolution_clock::now();
        Vector r2 = A | x;
        auto col_end = std::chrono::high_resolution_clock::now();

        std::printf("%u x %u:\tt_row = %.8gs,\tt_col = %.8gs,\tt_row/t_col = %.8g\n",
            size,
            size,
            std::chrono::duration<double>(row_end - row_start).count(),
            std::chrono::duration<double>(col_end - col_start).count(),
            std::chrono::duration<double>(row_end - row_start).count()/
            std::chrono::duration<double>(col_end - col_start).count()
        );

        (void)r;
    }
}