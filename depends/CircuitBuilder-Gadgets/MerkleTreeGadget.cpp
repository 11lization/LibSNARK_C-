//
// Created by jk on 22. 1. 28..
//

#include "MerkleTreeGadget.hpp"

#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>
#include <vector>

#include "hash/HashGadget.hpp"

namespace CircuitBuilder {
    namespace Gadgets {

        void MerkleTreeGadget::buildCircuit() {
            // const int leafWireLength = leafWires.size();
            unsigned long long int ptr = 0;

            Wires treeNodes = leafWires;
            HashGadget *hashGadget;

            // [0,1,2,3] [4:0||1, 5:2||3] [6:4||5]
            while (ptr+1 < treeNodes.size()) {
                WirePtr leftNode = treeNodes[ptr];
                WirePtr rightNode = treeNodes[ptr + 1];

                vector <WirePtr> hashInput = {leftNode, rightNode};
                hashGadget = allocate<HashGadget>(generator, hashInput);

                treeNodes = Util::concat(treeNodes ,hashGadget->getOutputWires());
                ptr += 2;
            }

            outWires = treeNodes;
        }


        MerkleTreeGadget::
        MerkleTreeGadget(CircuitGenerator *generator,
                         Wires &_leafWires,
                         int _height,
                         string desc)
                : Gadget(generator, desc), outWires(1) {
            leafWires = _leafWires;
            height = _height;
            if (leafWires.size() != static_cast<size_t>(height) ) {
                throw invalid_argument("leafWire length must be same tree height.");
            }
            buildCircuit();
        }

        MerkleTreeGadget::
        MerkleTreeGadget(CircuitGenerator *generator,
                         Wires &_leafWires,
                         string desc)
                : Gadget(generator, desc), outWires(1) {
            leafWires = _leafWires;
            height = (leafWires.size());
            buildCircuit();
        }

        Wires &MerkleTreeGadget::getOutputWires() {
            return outWires;
        }

        WirePtr &MerkleTreeGadget::getRootNode() {
            return outWires[outWires.size() - 1];
        }

    }
}