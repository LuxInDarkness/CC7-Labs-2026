#include "../Libraries/stdio.h"

#define BUFFER_SIZE 32

void main() {
    char input1[BUFFER_SIZE];
    char input2[BUFFER_SIZE];
    int num1, num2;
    float float1, float2;

    print("Program: Add Two Numbers\n");

    while (1) {
        // Prompt for first number
        read(input1, sizeof(input1), "Enter first number: ");
        num1 = alpha2int(input1);

        // Prompt for second number
        read(input2, sizeof(input2), "Enter second number: ");
        num2 = alpha2int(input2);

        // Display the result
        print("Sum: %d, Diff: %d, Mult: %d, Div: %d\n", num1 + num2, num1 - num2, num1 * num2, num1 / num2);

        read(input1, sizeof(input1), "Enter first float number: ");
        float1 = alpha2float(input1);

        read(input2, sizeof(input2), "Enter second float number: ");
        float2 = alpha2float(input2);

        print("Sum: %f, Diff: %f, Mult: %f, Div: %f\n", float1 + float2, float1 - float2, float1 * float2, float1 / float2);
    }
}