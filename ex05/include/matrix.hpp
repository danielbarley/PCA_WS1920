#ifndef __MATRIX_HPP_
#define __MATRIX_HPP_

#include <valarray>
#include <iostream>
#include <iomanip>

//#include <pthread.h>
#include <thread>

class Vector {
    std::valarray<double> data;
    public:
    Vector(unsigned int dim, bool random = false): data(dim) {
        if (random) {
            for (auto &x: data) {
                x = rand() %100;
            }
        }
    }

    Vector(std::valarray<double> v): data(v){}

    size_t size() {
        return data.size();
    }

    // vector multiplication
    double operator* (Vector v) {
        double o = 0;
        for (size_t i = 0; i < v.size(); i++) {
            o += v[i] * this->data[i];
        }
        return o;
    }

    // element-wise multiplication
    Vector operator| (Vector v) {
        Vector o = Vector(v.size());
        for (size_t i = 0; i < v.size(); i++) {
            o[i] = v[i] * this->data[i];
        }
        return o;
    }

    Vector operator+ (Vector v) {
        Vector o = Vector(v.size());
        for (size_t i = 0; i < v.size(); i++) {
            o[i] = v[i] + data[i];
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
    size_t nthreads;
    public:
        Matrix(unsigned int num_col, unsigned int num_row, bool random = false, size_t nthreads = 4)
        : nrow(num_row),
          ncol(num_col),
          data(num_col * num_row),
          nthreads(nthreads){
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
        Vector operator() (unsigned int col) {
            return (Vector)data[std::slice(col*nrow, nrow, 1)];
        }
        //std::slice_array<double> operator() (unsigned int col) {
            //return data[std::slice(col*nrow, nrow, 1)];
        //}

        // accessing col
        Vector operator[] (unsigned int row) const {
            return (std::valarray<double>)data[std::slice(row, ncol, nrow)];
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
        Vector operator*(Vector v) {
            Vector o = Vector(ncol);
            std::vector<std::thread> threads;
            threads.resize(nthreads);
            for (int x = 0; x <= (ncol / nthreads); x++) {
                for (size_t i = 0; i < std::min(nthreads, (ncol - x * nthreads)); i++) {
                    threads[i] = (std::thread(
                        [&] {
                            o[x * nthreads + i] = v * (*this)(x * nthreads + i);
                        }
                    ));
                }
                for (auto &thd: threads) {
                    if (thd.joinable())
                        thd.join();
                }
            }
            //for (size_t i = 0; i < this->ncol; i++) {
                //o[i] = v * (std::valarray<double>)((*this)(i));
            //}
            return o;
        }

        //Variant 2: column-wise access
        Vector operator| (Vector v) {
            std::vector<Vector> o(ncol, Vector(ncol));

            std::vector<std::thread> threads(nthreads);
            Vector out = Vector(ncol);
            for (int x = 0; x <= (ncol / nthreads); x++) {
                for (size_t i = 0; i < std::min(nthreads, (ncol - x * nthreads)); i++) {
                    threads[i] = (std::thread(
                        [&] {
                            o[i + x * nthreads] = v | (*this)(x * nthreads + i);
                        }
                    ));
                }
                for (auto &thd: threads) {
                    if (thd.joinable())
                        thd.join();
                }
            }
            for (auto p: o){
                out = out + p;
            }
            return out;
        }
};
#endif //__MATRIX_HPP_
