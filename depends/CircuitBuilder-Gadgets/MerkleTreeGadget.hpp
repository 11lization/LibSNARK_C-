//
// Created by jk on 22. 1. 28..
//

#ifndef LIBSNARK_OPTIMIZATION_MERKLETREEGADGET_H
#define LIBSNARK_OPTIMIZATION_MERKLETREEGADGET_H

#include <global.hpp>
#include <Config.hpp>
#include <Gadget.hpp>
#include <ConstantWire.hpp>
#include <CircuitGenerator.hpp>
#include <utilities.hpp>


namespace CircuitBuilder {
    namespace Gadgets {

        class MerkleTreeGadget : public CircuitBuilder::Gadget {

        private:

            Wires outWires;
            Wires leafWires;

            int height;

            void buildCircuit();

        public:

            /**
             *
             * @param generator
             * @param leafWires     Array of leafNode
             * @param height        Tree Height
             * @param desc
             */
            MerkleTreeGadget(CircuitGenerator *generator, Wires &leafWires, int height, string desc = "");

            /**
             * height = leafNode.length
             * @param generator
             * @param leafWires     Array of leafNode
             * @param desc
             */
            MerkleTreeGadget(CircuitGenerator *generator, Wires &leafWires, string desc = "");

            Wires &getOutputWires();
            WirePtr &getRootNode();

        };

    }
}

#endif //LIBSNARK_OPTIMIZATION_MERKLETREEGADGET_H
