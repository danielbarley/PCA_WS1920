#include <stdlib.h>
#include <math.h>
#include <string>
#include <valarray>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

#include "export.hpp"
#include "timer.hpp"

#include "mpi.h"

void initializeRing(
        std::vector<double> & timed_grid,
        size_t size,
        size_t xoff,
        size_t yoff,
        uint32_t diameter,
        double heat = 127
        ) {
    double cx, cy;
    cx = cy = size/2;

    for (uint32_t row = yoff; row < size; row++){
        for(uint32_t col = xoff; col < size; col++){
            double d = sqrt(pow(abs(cx - col),2) + pow(abs(cy - row),2));
            if ( abs(d - diameter/2) < .5 ) timed_grid[col+row*size] = heat;
        }
    }
}

void iterate(std::vector<double> & t0_grid, std::vector<double> & t1_grid, size_t size) {
    for (uint32_t row = 1; row < size-1; row++){
        for(uint32_t col = 1; col < size-1; col++){

            t1_grid[col+row*size] = t0_grid[col+row*size]
                + 6./25 * (
                    -4 * t0_grid[col+row*size]
                    + t0_grid[col+1+row*size]
                    + t0_grid[col-1+row*size]
                    + t0_grid[col+(row+1)*size]
                    + t0_grid[col+(row-1)*size]
                );
        }
    }
}

int main(int argc, const char** argv) {

    MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    std::vector<std::map<std::string,uint32_t>> givens {
        {{"size", 4}, {"diameter", 35}},
            {{"size", 512}, {"diameter", 175}},
            {{"size", 1024}, {"diameter", 350}},
            {{"size", 5120}, {"diameter", 1750}},
            {{"size", 10240}, {"diameter", 3500}},
    };

    constexpr size_t to_keep = 5;

    auto given = givens[atoi(argv[1])];
    int nodesize = given["size"]/std::sqrt(world_size);

    if (world_rank == 0){
        printf("[%s:%d] running w/ size of %dx%d\n",
                processor_name, world_rank, given["size"], given["size"]);

        printf("[%s:%d] slpit into %d nodes, each processing %dx%d\n",
                processor_name, world_rank, world_size, nodesize, nodesize);
    }

    std::array<std::vector<double>, to_keep> grid {}; // only a given no of timesteps will be kept

    for (auto & g: grid)
        g.resize((nodesize+2)*(nodesize+2), 0); // halo

    int xoff = nodesize*(world_rank%((int)std::sqrt(world_size)));
    int yoff = nodesize*(world_rank/((int)std::sqrt(world_size)));
    initializeRing(grid[0], given["size"], xoff, yoff, given["diameter"]);

    printf("[%s:%d] %dx%d+%d+%d\n", processor_name, world_rank, nodesize, nodesize, xoff, yoff);

    int edgenum = std::sqrt(world_size);

    bool has_n = world_rank / edgenum > 0;
    bool has_e = world_rank % edgenum < edgenum - 1;
    bool has_w = world_rank % edgenum > 0;
    bool has_s = world_rank / edgenum < edgenum - 1;
    bool is_ec = (world_rank % edgenum) % 2 == 0;
    bool is_er = (world_rank / edgenum) % 2 == 0;

    timer::Precision t1 {};
    for (uint32_t iter = 0; iter < 1000; iter++) {
        iterate(grid[iter%to_keep], grid[(iter+1)%to_keep], nodesize+2);
        //if (iter < 15){ // we dont need all images
        //std::stringstream ss;
        //ss << std::setfill('0') << "dat/" << given["size"] << "_t" << std::setw(4) << iter << ".pgm";
        //Export::PGM pgm = Export::PGM(ss.str(), grid[iter%to_keep], given["size"], 127);
        //pgm.flush();
        //}


        // iterate over all edge points
        for (size_t haloslot = 0; haloslot < nodesize; haloslot++){
            // for all 4 dir if possible
            for (int i = 0; i< 4; i++){
                // if i=0 and even col or i=1 and odd col
                // and only if we have a west neighbor
                if (((i == 0 && is_ec) || (i == 1 && !is_ec)) && has_w){
                    MPI_Sendrecv(
                            &grid[(iter+1)%to_keep][(nodesize+2)*(haloslot+1)+1],
                            1,
                            MPI_DOUBLE,
                            world_rank-1,
                            0,
                            &grid[(0+1)%to_keep][(nodesize+2)*(haloslot+1)],
                            1,
                            MPI_DOUBLE,
                            world_rank-1,
                            0,
                            MPI_COMM_WORLD,
                            NULL
                            );
                }
                if (((i == 0 && !is_ec) || (i == 1 && is_ec)) && has_e){ // has East Neighbor
                    MPI_Sendrecv(
                            &grid[(iter+1)%to_keep][(nodesize+2)*(haloslot+2)-2],
                            1,
                            MPI_DOUBLE,
                            world_rank+1,
                            0,
                            &grid[(0+1)%to_keep][(nodesize+2)*(haloslot+2)-1],
                            1,
                            MPI_DOUBLE,
                            world_rank+1,
                            0,
                            MPI_COMM_WORLD,
                            NULL
                            );
                }
                if (((i == 2 && is_er) || (i==3 && !is_er)) && has_n ){ // has north Neighbor
                    MPI_Sendrecv(
                            &grid[(iter+1)%to_keep][1+haloslot+(nodesize+2)],
                            1,
                            MPI_DOUBLE,
                            world_rank-edgenum,
                            0,
                            &grid[(0+1)%to_keep][1+haloslot],
                            1,
                            MPI_DOUBLE,
                            world_rank-edgenum,
                            0,
                            MPI_COMM_WORLD,
                            NULL
                            );
                }
                if (((i==2 && !is_er) || (i==3 && is_er)) && has_s ){ // has south Neighbor
                    MPI_Sendrecv(
                            &grid[(iter+1)%to_keep][1+haloslot+(nodesize+2)*(nodesize-1)],
                            1,
                            MPI_DOUBLE,
                            world_rank+edgenum,
                            0,
                            &grid[(0+1)%to_keep][1+haloslot+(nodesize+2)*nodesize],
                            1,
                            MPI_DOUBLE,
                            world_rank+edgenum,
                            0,
                            MPI_COMM_WORLD,
                            NULL
                            );
                }
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    t1.stop();

    printf("[%s:%d] done (in %fs)\n", processor_name, world_rank, t1.duration());

    Export::Export ex = Export::Export("dat/dat.dat");
    ex.from_df({{given["size"]}, {t1.duration()}});
    ex.flush();
    MPI_Finalize();
}
