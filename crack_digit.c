// Project: COMP1511 Assignment 1// Brief: C Program that outputs a prediction of a number for the command line argument input of an image filename// Author: Da Jason Sun (z5059224) // Contact details: jason.sun@student.unsw.edu.au// Date: 30/04/2017// Notes: This file also contains training and testing functions for the digit predictions#include <stdio.h>#include <stdlib.h>#include <math.h>#include "captcha.h"void train_neural_network (layer_t *nNLayer);void test_neural_network (layer_t *nNLayer);int main (int argc, char *argv[]) {    int height = 0, width = 0, start_row = 0, start_column = 0, box_width = 0, box_height = 0;    int prediction = 0;    int centred_pixels[NN_HEIGHT][NN_WIDTH] = {{0}};    layer_t nNLayer;        // Train/test functions to determine weights and test accuracy of predictions    // train_neural_network (&nNLayer);    // test_neural_network (&nNLayer);    // Initialises neural network layer and initialises weights to previously trained values        get_initialised_layer (&nNLayer);    get_trained_weights (&nNLayer);        // Produce error message if no image is called on command line    // Taken from the testing files from lab exercise 7    if (argc < 2) {        fprintf(stderr, "Usage: %s <image-file>\n", argv[0]);        return 1;    }    // Exits program if unable to get pbm dimensions from image    // Taken from the testing files from lab exercise 7    if (get_pbm_dimensions (argv[1], &height, &width) != 1) {        return 1;    }    // Declares pixels after height and width are known for array size    int pixels[height][width];        // Reads input and determines prediction through centring bounding box and determining the highest neuron output    // Reads input    if (read_pbm(argv[1], height, width, pixels) == 1) {        // Centres bounding box of inputted digit        get_centred_digit (height, width, pixels, centred_pixels);        // Loops through all 10 neurons for input and then output        for (int neuronTicker = 0; neuronTicker < N_SINGLE_DIGITS; neuronTicker ++) {            get_neuron_input (&nNLayer.neuron[neuronTicker], NN_HEIGHT, NN_WIDTH, centred_pixels);            get_neuron_output (&nNLayer.neuron[neuronTicker]);        }        // Determines prediction from highest value output out of all 10 neurons        prediction = get_prediction (&nNLayer);        printf ("%d\n", prediction);    }    return EXIT_SUCCESS;}// Trains neural network through looping through the training functions that undergo supervised learningvoid train_neural_network (layer_t *nNLayer) {    char fileName[FILENAME_STRING_LENGTH] = {0};    int fileLabel = 0;    double neuronError = 0;    int height = 0, width = 0;    int prediction = 0;    int nSuccess = 0;    int centred_pixels[NN_HEIGHT][NN_WIDTH] = {{0}};    output_format_t targetOutput;    // Opening file with training file names and file labels    FILE *digitFileNames = fopen ("digits_file_names.c", "r");    FILE *digitFileLabels = fopen ("digits_file_labels.c", "r");    FILE *trainedWeights = fopen ("trained_weights.txt", "w");    // Initialise the neural network    get_initialised_layer (nNLayer);    for (int filesTicker = 0; filesTicker < N_TRAINING_FILES; filesTicker ++) {        // Reading file names and file labels        fscanf (digitFileNames, "%s", fileName);        fscanf (digitFileLabels, "%d", &fileLabel);        // Sets file label in output format that can be interpreted by the neural network        targetOutput = get_target_output (fileLabel);        // Determines pbm dimensions before declaring pixels for the known height and width array size        get_pbm_dimensions (fileName, &height, &width);        int pixels[height][width];        // Reads input        read_pbm (fileName, height, width, pixels);        // Centres bounding box of inputted digit        get_centred_digit (height, width, pixels, centred_pixels);                // Training neural network throuhg looping through each neuron and determining input, output,         // neuron error and then updating neuron weights given the neuron error        for (int neuronTicker = 0; neuronTicker < N_SINGLE_DIGITS; neuronTicker ++) {            get_neuron_input (&nNLayer->neuron[neuronTicker], NN_HEIGHT, NN_WIDTH, centred_pixels);            get_neuron_output (&nNLayer->neuron[neuronTicker]);            neuronError = get_neuron_error (&nNLayer->neuron[neuronTicker], targetOutput.outputValue[neuronTicker]);            update_neuron_weights (&nNLayer->neuron[neuronTicker], neuronError);        }    }    // Hardcoding the trained weights    for (int neuronTicker = 0; neuronTicker < N_SINGLE_DIGITS; neuronTicker ++) {        for (int y = 0; y < NN_HEIGHT; y ++) {            for (int x = 0; x < NN_WIDTH; x ++) {                fprintf (trainedWeights, "%lf\n", nNLayer->neuron[neuronTicker].weight[y][x]);            }        }    }    // Closing opened files    fclose (digitFileNames);    fclose (digitFileLabels);    fclose (trainedWeights);}// Tests neural network on digits to test accuracy of predictionsvoid test_neural_network (layer_t *nNLayer) {    char fileName[FILENAME_STRING_LENGTH] = {0};    int fileLabel = 0;    double neuronError = 0;    int height = 0, width = 0;    int prediction = 0;    int nSuccess = 0;    int centred_pixels[NN_HEIGHT][NN_WIDTH] = {{0}};    output_format_t targetOutput;        // Opening file with training file names and file labels    FILE *digitFileNames = fopen ("test_digits_file_names.c", "r");    FILE *digitFileLabels = fopen ("test_digits_file_labels.c", "r");    // Initialising the weights to the already trained values    get_initialised_layer (nNLayer);    get_trained_weights (nNLayer);    // Running through testing files    for (int filesTicker = 0; filesTicker < N_TESTING_FILES; filesTicker ++) {        // Reading file names and file labels        fscanf (digitFileNames, "%s", fileName);        fscanf (digitFileLabels, "%d", &fileLabel);        // Sets file label in output format that can be interpreted by the neural network        targetOutput = get_target_output (fileLabel);        // Determines pbm dimensions before declaring pixels for the known height and width array size        get_pbm_dimensions (fileName, &height, &width);        int pixels[height][width];        // Reads input        read_pbm (fileName, height, width, pixels);        // Centres bounding box of inputted digit        get_centred_digit (height, width, pixels, centred_pixels);        // Tests predictions by looping through all 10 neurons for input and then output        for (int neuronTicker = 0; neuronTicker < N_SINGLE_DIGITS; neuronTicker ++) {            get_neuron_input (&nNLayer->neuron[neuronTicker], NN_HEIGHT, NN_WIDTH, centred_pixels);            get_neuron_output (&nNLayer->neuron[neuronTicker]);        }        // Calculating and displaying aggregate resutls through multiple tests        prediction = get_prediction (nNLayer);        if (prediction == fileLabel) {            nSuccess ++;        }        printf ("%s\n", fileName);        printf ("prediction = %d, fileLabel = %d\n", prediction, fileLabel);        printf ("Success rate = %lf\n\n", nSuccess / (double) (filesTicker + 1));    }    // Closing opened files    fclose (digitFileNames);    fclose (digitFileLabels);}