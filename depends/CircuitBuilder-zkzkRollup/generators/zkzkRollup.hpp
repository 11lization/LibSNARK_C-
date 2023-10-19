

#pragma once


#include <global.hpp>
#include <Config.hpp>
#include <CircuitGenerator.hpp>

namespace CircuitBuilder {
    namespace zkzkRollup {

        class zkzkRollup : public CircuitBuilder::CircuitGenerator {

        private:

            /********************* INPUT ***************************/
            WirePtr targetNode; // leafNodes's root and batch tree's leaf node.
            WirePtr rt; // batch tree's root.
            WiresPtr leafNodes;

            /********************* Witness ***************************/

            /********************* MerkleTree ***************************/
            WirePtr directionSelector;
            WiresPtr intermediateHashWires;
            int treeHeight;

        public:

        protected:

            void buildCircuit();

        public:

            zkzkRollup(string circuitName, int treeHeight , Config &config);

            void assignInputs(CircuitEvaluator &evaluator);

        };
    }
}