#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define REG_WRITE     		_IOW('k', 1, struct ioctl_data)
#define REG_READ      		_IOR('k', 2, struct ioctl_data)
#define SEL_FREQ_PLAN 		_IOW('k', 3, unsigned int)
#define CLK_ENABLE    		_IOW('k', 4, unsigned int)
#define CLK_DISABLE   		_IOW('k', 5, unsigned int)
#define CLK_SELECT_FREQ 	_IOW('k', 6, struct ioctl_data)
#define INPUT_CLK     		_IOWR('k', 7, struct ioctl_data)
#define OUTPUT_CLK    		_IOWR('k', 8, struct ioctl_data)
#define SET_FREQUENCY 		_IOW('k', 9, struct freq_config)
#define SET_INPUT_FREQ 		_IOW('k', 10, struct freq_config)

#define CONFIGURE_OUTPUT_CLOCK_LINK_TYPE 	_IOW('k', 11, struct output_clock_config)
#define CONFIGURE_INPUT_CLOCK_LINK_TYPE		_IOW('k', 12, struct input_clock_config)

struct freq_config {
	unsigned int input_frequency;
	unsigned int output_frequency;
	int clkid;
};

struct ioctl_data {
	unsigned int page;
	unsigned int clk_id;
	unsigned int reg_address;
	unsigned int value;
};

struct output_clock_config {
	int clkid;
	unsigned int pllA;
	unsigned int pllB;
	unsigned int pllC;
	unsigned int pllD;
	unsigned int linktype;
	unsigned int difflinktype;
	unsigned int mode;
	unsigned int swing;
	unsigned int itresistor;
};

struct input_clock_config{
	int clkid;
	unsigned int pllA;
	unsigned int pllB;
	unsigned int pllC;
	unsigned int pllD;
	unsigned int linktype;
	unsigned int difflinktype;
	unsigned int mode;
};

#define DEVICE_FILE "/dev/SiT9531xDrv"

void print_menu() {
	printf("Select operation:\n");
	printf("1. Read from register\n");
	printf("2. Write to register\n");
	printf("3. Select frequency plan\n");
	printf("4. Enable clock\n");
	printf("5. Disable clock\n");
	printf("6. Select clock frequency\n");
	printf("7. Set Input clock frequency\n");
	printf("8. Configure Output Clock\n");
	printf("9. Configure Input Clock\n");
	printf("10. Exit\n");
	printf("Enter your choice: ");
}

int main() {
	int fd;
	int freq_choice;
	int type_choice;
	int mode_choice;
	int swing_choice;
	int pll_choice;
	int pll_input;
	int pn_choice;
	int coupling_choice;
	
	char pll_names[5] = { 0 }; 
	int num_plls = 0;
	struct ioctl_data data;
	struct freq_config config;
	struct input_clock_config input_config;
	struct output_clock_config output_config;
	int choice;

	// Open the device file
	fd = open(DEVICE_FILE, O_RDWR);
	if (fd < 0) {
		perror("Cannot open device");
		return EXIT_FAILURE;
	}

	while (1) {
		print_menu();
		scanf("%d", &choice);

		if (choice == 10) {
			break;
		}

		switch (choice) {
			case 1: // Read from register
				printf("Enter register address: ");
				scanf("%x", &data.reg_address);

				if (ioctl(fd, REG_READ, &data) < 0) {
					perror("IOCTL REG_READ failed");
				} else {
					printf("Read value: 0x%x from register 0x%x\n", data.value, data.reg_address);
				}
				break;

			case 2: // Write to register
				printf("Enter register address: ");
				scanf("%x", &data.reg_address);

				printf("Enter value to write: ");
				scanf("%x", &data.value);

				if (ioctl(fd, REG_WRITE, &data) < 0) {
					perror("IOCTL REG_WRITE failed");
				} else {
					printf("Wrote value: 0x%x to register 0x%x\n", data.value, data.reg_address);
				}
				break;

			case 3: // Select frequency plan
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
				scanf("%u", &data.clk_id);

				if (ioctl(fd, CLK_ENABLE, &data.clk_id) < 0) {
					perror("IOCTL CLK_ENABLE failed");
				} else {
					printf("Clock %u enabled successfully.\n", data.clk_id);
				}
				break;

			case 5: // Disable clock
				printf("Enter clock ID to disable: ");
				scanf("%u", &data.clk_id);

				if (ioctl(fd, CLK_DISABLE, &data.clk_id) < 0) {
					perror("IOCTL CLK_DISABLE failed");
				} else {
					printf("Clock %u disabled successfully.\n", data.clk_id);
				}
				break;

			case 6: // Select clock frequency
				printf("Enter clock ID: ");
				scanf("%d", &config.clkid);

				printf("Enter frequency to set (in Hz): ");
				scanf("%u", &config.output_frequency);

				if (ioctl(fd, SET_FREQUENCY, &config) < 0) {
					perror("IOCTL CLK_SELECT_FREQ failed");
				} else {
					printf("Frequency %u Hz set for clock ID %d.\n", config.output_frequency, config.clkid);
				}
				break;

			case 7: // Set Input clock frequency
				printf("Enter clock ID: ");
				scanf("%d", &config.clkid);

				printf("Enter input frequency (in Hz): ");
				scanf("%u", &config.input_frequency);

				printf("Enter output frequency (in Hz): ");
				scanf("%u", &config.output_frequency);

				if (ioctl(fd, SET_INPUT_FREQ, &config) < 0) {
					perror("IOCTL SET_INPUT_FREQ failed");
				} else {
					printf("Input frequency set to %u Hz, Output frequency set to %u Hz for clock ID %d.\n",
							config.input_frequency, config.output_frequency, config.clkid);
				}
				break;

			case 8: // Configure Output Clock
				printf("Enter clock ID: ");
				scanf("%d", &output_config.clkid);

				printf("Select clock type:\n");
				printf("1. Single-Ended\n");
				printf("2. Differential\n");
				printf("Enter your choice: ");
				scanf("%d", &type_choice);

				if (type_choice == 1) {
					output_config.linktype = 0; // Single-Ended
					printf("Single-Ended clock selected. No additional configurations required.\n");
				} 
				else if (type_choice == 2) // Differential
				{
					output_config.linktype = 1; 

					//const char[4][10] = {"LVDS","LVPECL","CML","HCSL"}; 
					printf("Select mode:\n");
					printf("1. LVDS\n");
					printf("2. LVPECL\n");
					printf("3. CML\n");
					printf("4. HCSL\n");
					printf("Enter your choice: ");
					scanf("%d", &mode_choice);
					output_config.mode = mode_choice-1;


					if (output_config.mode == 0)
					{
						printf("Enable internal termination? (1: Yes, 0: No): ");
						scanf("%d", &output_config.itresistor);
					} 
					else
					{
						output_config.itresistor = 0; // Not applicable for other modes
					}

					printf("Select swing voltage:\n");
					printf("1. 100mV\n");
					printf("2. 200mV\n");
					printf("3. 300mV\n");
					printf("4. 400mV\n");
					printf("5. 500mV\n");
					printf("6. 600mV\n");
					printf("7. 700mV\n");
					printf("8. 800mV\n");
					printf("Enter your choice: ");
					scanf("%d", &swing_choice);

					switch (swing_choice) {
						case 1: output_config.swing = 100; break;
						case 2: output_config.swing = 200; break;
						case 3: output_config.swing = 300; break;
						case 4: output_config.swing = 400; break;
						case 5: output_config.swing = 500; break;
						case 6: output_config.swing = 600; break;
						case 7: output_config.swing = 700; break;
						case 8: output_config.swing = 800; break;
						default:
							printf("Invalid swing voltage selected. Defaulting to 100mV.\n");
							output_config.swing = 100;
					}
				} 
				else { // Neither SE or Differential
					printf("Invalid clock type selected.\n");
					break;
				}

				printf("Select PLL:\n");
				printf("1. PLL A\n");
				printf("2. PLL B\n");
				printf("3. PLL C\n");
				printf("4. PLL D\n");
				printf("Enter your choice: ");
				scanf("%d", &pll_choice);

				output_config.pllA = 0;
				output_config.pllB = 0;
				output_config.pllC = 0;
				output_config.pllD = 0;

				switch (pll_choice) {
					case 1: output_config.pllA = 1; break;
					case 2: output_config.pllB = 1; break;
					case 3: output_config.pllC = 1; break;
					case 4: output_config.pllD = 1; break;
					default:
						printf("Invalid PLL selected. Defaulting to PLLA.\n");
						output_config.pllA = 1;
				}

        printf("application cmd id : %d",CONFIGURE_OUTPUT_CLOCK_LINK_TYPE);
				if (ioctl(fd, CONFIGURE_OUTPUT_CLOCK_LINK_TYPE, &output_config) < 0) {
					perror("IOCTL CONFIGURE_OUTPUT_CLOCK_LINK_TYPE failed");
				} else {
					printf("Output clock  link type configured successfully.\n");
				}


				break;

			case 9: // Configure Input Clock
				printf("Enter clock ID: ");
				scanf("%d", &input_config.clkid);

				printf("Select clock type:\n");
				printf("1. Single-Ended\n");
				printf("2. Differential\n");
				printf("Enter your choice: ");

				scanf("%d", &type_choice);

				if (type_choice == 1) // Single-Ended
				{
					input_config.linktype = 0; 
					printf("Select type:\n");
					printf("1. P \n");
					printf("2. N \n");
					scanf("%d", &pn_choice);
					if (pn_choice == 1) {
						input_config.difflinktype = 0; // P
					} 
					else if (pn_choice == 2) {
						input_config.difflinktype = 1; // N
					}

					printf("Single-Ended clock selected. No additional configurations required.\n");
				} 
				else if (type_choice == 2)  // Differential
				{ 
					input_config.linktype = 1;


					printf("Select coupling:\n");
					printf("1. for AC\n");
					printf("2. for DC\n");
					printf("Enter your choice: ");
					scanf("%d", &coupling_choice);	
					if (type_choice == 1) {
						input_config.mode = 0; // AC
					} 
					else if (type_choice == 2) {
						input_config.mode = 1; // DC
					}

				}

				printf("Select PLL(s) (you can select multiple):\n");
				printf("1. PLL A\n");
				printf("2. PLL B\n");
				printf("3. PLL C\n");
				printf("4. PLL D\n");
				printf("Enter your choice(s) : ");

				while (1) {
					scanf("%d", &pll_input);
					if (pll_input == 0) break; 

					if (pll_input >= 1 && pll_input <= 4) {
						char pll_name = 'A' + (pll_input - 1);


						if (strchr(pll_names, pll_name) == NULL) { 
							pll_names[num_plls++] = pll_name;
						}
					}
					else {
						printf("Invalid PLL selected. Enter 0 to finish or choose again (1-4).\n");
					}
				}

				pll_names[num_plls] = '\0'; 

				if (num_plls > 0) {
					printf("Selected PLL(s): %s\n", pll_names);

					for(int i = 0; i < 4; i++)
					{        
						if(pll_names[i] == 'A' )
						{
							input_config.pllA = 1;
						}
						else if(pll_names[i] == 'B' )
						{
							input_config.pllB = 1;
						}
						else if(pll_names[i] == 'C' )
						{
							input_config.pllC = 1;
						}
						else if(pll_names[i] == 'D' )
						{
							input_config.pllD = 1;
						}
					}

				}
				else {
					printf("No PLLs selected. Skipping PLL configuration.\n");
				}
				
				if (ioctl(fd, CONFIGURE_INPUT_CLOCK_LINK_TYPE, &input_config) < 0) {
					perror("IOCTL CONFIGURE_INPUT_CLOCK_LINK_TYPE failed");
				} else {
					printf("Input clock  link type configured successfully.\n");
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
