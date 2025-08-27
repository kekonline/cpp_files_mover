# Use official GCC image
FROM gcc:latest

# Create working directory inside container
WORKDIR /usr/src/app

# Copy project files into container
COPY . .

# Default command: compile and run main.cpp
CMD ["bash", "-c", "g++ main.cpp -o main && ./main"]


