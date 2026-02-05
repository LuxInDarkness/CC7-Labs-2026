#include "stdio.h"

void main() {
    char *input1;
    char *input2;
    int num1, num2, intTotal;
    float float1, float2, flTotal;

    print("Program: Add Two Numbers\n");

    while (1) {
        // Prompt for first number
        read(input1, "Enter first number: ");
        num1 = alpha2int(input1);

        // Prompt for second number
        read(input2, "Enter second number: ");
        num2 = alpha2int(input2);

        // Calculate sum
        intTotal = num1 + num2;

        // Display the result
        print("Sum: %d\n", intTotal);

        read(input1, "Enter first float number: ");
        float1 = alpha2float(input1);

        read(input2, "Enter second float number: ");
        float2 = alpha2float(input2);

        flTotal = float1 + float2;

        print("Sum: %f\n", flTotal);
    }
}
