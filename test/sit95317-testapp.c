#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>


#define REG_WRITE     _IOW('k', 1, struct ioctl_data)
#define REG_READ      _IOR('k', 2, struct ioctl_data)
#define SEL_FREQ_PLAN _IOW('k', 3, unsigned int)
#define CLK_ENABLE     _IOW('k', 4, unsigned int)
#define CLK_DISABLE    _IOW('k', 5, unsigned int)
#define CLK_SELECT_FREQ _IOW('k', 6, struct ioctl_data)
#define INPUT_CLK 	_IOWR('k', 7, struct ioctl_data)
#define OUTPUT_CLK 	_IOWR('k', 8, struct ioctl_data)
#define SET_FREQUENCY _IOW('k', 9,struct freq_config)
#define SET_INPUT_FREQ _IOW('k', 10, struct freq_config)

struct freq_config{

    unsigned int input_frequency;  
    unsigned int output_frequency; 
    int clkid;                     
};

struct ioctl_data {
    unsigned int page;
    unsigned int clk_id; //  Clock ID for enable/disable or frequency selection
    unsigned int reg_address;
    unsigned int value;  // This will store the value read from the register
};



#define DEVICE_FILE "/dev/SiT9531xDrv"

void print_menu() {
    printf("Select operation:\n");
    printf("1. Read from register\n");
    printf("2. Write to register\n");
    printf("3. Select  frequency_plan\n");
    printf("4. Enable clock\n");
    printf("5. Disable clock\n");
    printf("6. Select clock frequency\n");
    printf("7. Set Input clock frequency\n");
    printf("8. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    int fd;
    struct ioctl_data data;
    unsigned int reg_address;
    unsigned int value;
    int choice;
    int freq_choice;
    unsigned int clk_id;
    struct freq_config config;
    unsigned int i;

    // Open the device file
    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Cannot open device");
        return EXIT_FAILURE;
    }

    while (1) {
        print_menu();
        scanf("%d", &choice);

        if (choice == 8) {
            break;
        }

        switch (choice) {
            case 1: // Read from register
                printf("Enter register address: ");
                scanf("%x", &reg_address);
                data.reg_address = reg_address;

                if (ioctl(fd, REG_READ, &data) < 0) {
                    perror("IOCTL REG_READ failed");
                } else {
                    printf("Read value: 0x%x from register 0x%x\n", data.value, data.reg_address);
                }
                break;

            case 2: // Write to register
                printf("Enter register address: ");
                scanf("%x", &reg_address);
                data.reg_address = reg_address;

                printf("Enter value to write: ");
                scanf("%x", &value);
                data.value = value;

                if (ioctl(fd, REG_WRITE, &data) < 0) {
                    perror("IOCTL REG_WRITE failed");
                } else {
                    printf("Wrote value: 0x%x to register 0x%x\n", data.value, data.reg_address);
                }
                break;

            case 3: // Select  frequency_plan
                printf("Select frequency:\n");
                

                printf("1. Frequency 1 (Def1 : 50MHz)\n");
                printf("2. Frequency 2 (Def2 : 1MHz,1PPS)\n");
              
                printf("Enter your choice: ");
                scanf("%d", &freq_choice);

                if (ioctl(fd, SEL_FREQ_PLAN, &freq_choice) < 0) {
                    perror("IOCTL SEL_FREQ_PLAN failed");
                } 
                break;
          case 4: // Enable clock
                printf("Enter clock ID to enable: ");
                scanf("%u", &clk_id);

                if (ioctl(fd, CLK_ENABLE, &clk_id) < 0) {
                    perror("IOCTL CLK_ENABLE failed");
                } else {
                    printf("Clock %u enabled successfully.\n", clk_id);
                }
                break;

            case 5: // Disable clock
                printf("Enter clock ID to disable: ");
                scanf("%u", &clk_id);

                if (ioctl(fd, CLK_DISABLE, &clk_id) < 0) {
                    perror("IOCTL CLK_DISABLE failed");
                } else {
                    printf("Clock %u disabled successfully.\n", clk_id);
                }
                break;

            case 6: // Select clock frequency

                printf("Enter clock ID: ");
                scanf("%x", &config.clkid);
                
                printf("Enter frequency to set (in Hz): ");
                scanf("%u", &config.output_frequency);

                if (ioctl(fd, SET_FREQUENCY, &config) < 0) {
                    perror("IOCTL CLK_SELECT_FREQ failed");
                } else {
                    printf("Frequency %u Hz set for clock ID %u.\n", config.output_frequency, config.clkid);
                }
                break;
                
                  case 7: // Set Input clock frequency
    printf("Enter clock ID: ");
    scanf("%d", &config.clkid);
    
    
    printf("Enter input frequency (in Hz): ");
    scanf("%u", &config.input_frequency);

    printf("Enter output frequency (in Hz): ");
    scanf("%u", &config.output_frequency);

    // Set input frequency via ioctl
    if (ioctl(fd, SET_INPUT_FREQ, &config) < 0) {
        perror("IOCTL SET_INPUT_FREQ failed");
    } else {
        printf("Input frequency set to %u Hz, Output frequency set to %u Hz for clock ID %d.\n",
               config.input_frequency, config.output_frequency, config.clkid);
    }
    break;

            default:
                printf("Invalid choice, please try again.\n");
        }

        printf("\n");
    }

    // Close the device file
    close(fd);

    return EXIT_SUCCESS;
}
