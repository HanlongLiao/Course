#include <iostream>
#include <WinSock2.h>
#include <string.h>
#define HAVE_REMOTE
#include <pcap.h>
#define LINE_LEN 16
#define MAX_ADDR_LEN 16

#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "Ws2_32.lib")


using namespace std;

//Etherent protocol format
typedef struct ether_header {
	u_char ether_dhost[6];
	u_char ether_shost[6];
	u_short ether_type;
}ether_header;

// IP
typedef struct ip_address {
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

//The header of IPV4
typedef struct ip_header {
	u_char version_hlen;
	u_char tos;
	u_short tlen;
	u_short identification;
	u_short flags_offset;
	u_char ttl;
	u_char proto;
	u_short checksum;
	u_int saddr;
	u_int daddr;
}ip_header;

//The header of TCP
typedef struct tcp_header {
	u_short sport;
	u_short dport;
	u_int sequence;
	u_int ack;
	u_char hdrLen;
	u_char flags;
	u_short windows;
	u_short checksum;
	u_short urgent_pointer;
}tcp_header;

//The header of UDP
typedef struct udp_header {
	u_short sport;
	u_short dport;
	u_short datalen;
	u_short checksum;
}udp_header;

//The header of ICMP
typedef struct icmp_header {
	u_char type;
	u_char code;
	u_short checksum;
	u_short identification;
	u_short sequence;
	u_long timestamp;
}icmp_header;

//The header of ARP
typedef struct arp_header {
	u_short hardware_type;					
	u_short protocol_type;					
	u_char hardware_length;					
	u_char protocol_length;					
	u_short operation_code;					
	u_char source_ethernet_address[6];		
	u_char source_ip_address[4];			
	u_char destination_ethernet_address[6];	
	u_char destination_ip_address[4];		
}arp_header;


//Funtion to anylyze packet as tcp
void tcp_protocol_packet_handle(
	u_char *arg, 
	const struct pcap_pkthdr *pkt_header, 
	const u_char *pkt_content);

//Funtion to anylyze packet as udp
void udp_protocol_packet_handle(
	u_char *arg, 
	const struct pcap_pkthdr *pkt_header, 
	const u_char *pkt_content);

//Function to anylyze packet as icmp
void icmp_protocol_packet_handle(
	u_char *arg, 
	const struct pcap_pkthdr *pkt_header, 
	const u_char *pkt_content);

//Funton to anylyze packet as arp
void arp_protocol_packet_handle(
	u_char *arg, 
	const struct pcap_pkthdr *pkt_header, 
	const u_char *pkt_content);

//Funtion to analyze packet as IP 
void IP_protocol_packet_handle(
	u_char *arg, 
	const struct pcap_pkthdr *pkt_header, 
	const u_char *pkt_content);

// Funtion to analyze packet as etherent 
void etherent_protocol_packet_handle(
	u_char *arg, 
	const struct pcap_pkthdr *pkt_header, 
	const u_char *pkt_content);


int main() {
	pcap_if_t *alldevs;
	pcap_if_t *d;
	pcap_t *adhandle;
	int inum; //The squeue number of adapters selected

	char errBuf[PCAP_ERRBUF_SIZE];

	int res; //The return value of pacp_open_ex()
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	cout << "The list of Adapters" << endl;
	if (pcap_findalldevs(&alldevs, errBuf) == -1) {
		cout << "Error at pcap_findalldevs() " << errBuf << endl;
		exit(1);
	}

	// Print the list of devs
	int i = 0;
	for (d = alldevs; d != NULL; d = d->next) {
		cout << d->name;
		i++;
		if (d->description) cout << d->description << endl;
		else cout << "No description!" << endl;
	}

	if (!i) {
		cout << "No interface is found! Please make sure you have installed Winpacp!" << endl;
		exit(-1);
	}

	//Select Adapter
	cout << "Enter the interface number: " << endl;
	while (true) {
		cin >> inum;
		if (inum <= i && inum >= 0) break;
		else cout << "The number shoud range at 1 ~ " << i << endl;
	}

	//Open adapter selected
	for (i = 1, d = alldevs; i <= inum; i++, d = d->next);
	if ((adhandle = pcap_open(
		d->name,
		65536,
		PCAP_OPENFLAG_PROMISCUOUS,
		1000,
		NULL,
		errBuf)) == NULL) {
		cout << "Error at pcap_open()" << endl;
		pcap_freealldevs(alldevs);
		exit(-1);
	}

	if (pcap_datalink(adhandle) != DLT_EN10MB) {
		cout << "This program noly run on Etherent network" << endl;
		pcap_close(adhandle);
		pcap_freealldevs(alldevs);
		exit(-1);
 	}

	//Grabbing and filtering
	
	time_t local_tv_sec;
	struct tm *ltime;
	char timestr[16];

	int count = 1;
	char temp[LINE_LEN + 1];
	memset(temp, 0, (LINE_LEN + 1) * sizeof(temp[0]));
	while ((res = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0) {
		if (!res) continue;

local_tv_sec = header->ts.tv_sec;
ltime = localtime(&local_tv_sec);
strftime(timestr, sizeof(timestr), "%H:%M:%S", ltime);
cout << endl << endl << endl;
cout << "-------------------------------------------------------------------------------------" << endl;
printf("No.%d\ttime: %s\tlen: %ld\n", count++, timestr, header->len);
cout << "-------------------------------------------------------------------------------------" << endl;
cout << endl;
// Print packet
cout << "==============The Packet Content================" << endl << endl;
for (i = 0; i < header->caplen; i++) {
	if (isgraph(pkt_data[i]) || pkt_data[i] == ' ') {
		temp[i % LINE_LEN] = pkt_data[i];
	}
	else temp[i % LINE_LEN] = '.';

	if (i % LINE_LEN == 15) {
		temp[16] = '\0';
		cout << temp << endl;
		memset(temp, 0, LINE_LEN + 1);
	}
}
cout << endl;

etherent_protocol_packet_handle(NULL, header, pkt_data);

	}


}


void tcp_protocol_packet_handle(
	u_char *arg,
	const struct pcap_pkthdr *pkt_header,
	const u_char *pkt_content
) {
	tcp_header *tcp_protocol;

	tcp_protocol = (tcp_header*)(pkt_content + 14 + 20);

	printf("===================TCP Protocol=================\n");
	cout << endl;
	printf("Source Port: %i\n", ntohs(tcp_protocol->sport));
	printf("Destination Port: %i\n", ntohs(tcp_protocol->dport));
	printf("Sequence number: %d\n", ntohl(tcp_protocol->sequence));
	printf("Acknowledgment number: %d\n", ntohl(tcp_protocol->ack));
	printf("Header Length: %d\n", (tcp_protocol->hdrLen >> 4) * 4);
	printf("Flags: 0x%.3x ", tcp_protocol->flags);
	if (tcp_protocol->flags & 0x08) printf("(PSH)");
	if (tcp_protocol->flags & 0x10) printf("(ACK)");
	if (tcp_protocol->flags & 0x02) printf("(SYN)");
	if (tcp_protocol->flags & 0x20) printf("(URG)");
	if (tcp_protocol->flags & 0x01) printf("(FIN)");
	if (tcp_protocol->flags & 0x04) printf("(RST)");
	printf("\n");
	printf("Windows Size: %i\n", ntohs(tcp_protocol->windows));
	printf("Checksum: 0x%.4x\n", ntohs(tcp_protocol->checksum));
	printf("Urgent Pointer: %i\n", ntohs(tcp_protocol->urgent_pointer));
	cout << endl;

}

void udp_protocol_packet_handle(
	u_char *arg,
	const struct pcap_pkthdr *pkt_header,
	const u_char *pkt_content) {
	udp_header *udp_protocol;

	udp_protocol = (udp_header*)(pkt_content + 14 + 20);

	printf("===================UDP Protocol=================\n");
	cout << endl;
	printf("Source Port: %i\n", ntohs(udp_protocol->sport));
	printf("Destination Port: %i\n", ntohs(udp_protocol->dport));
	printf("Datalen: %i\n", ntohs(udp_protocol->datalen));
	printf("Checksum: 0x%.4x\n", ntohs(udp_protocol->checksum));
	cout << endl;
}

void icmp_protocol_packet_handle(
	u_char *arg,
	const struct pcap_pkthdr *pkt_header,
	const u_char *pkt_content) {
	icmp_header *icmp_protocol;

	icmp_protocol = (icmp_header*)(pkt_content + 14 + 20);

	printf("==================ICMP Protocol=================\n");
	cout << endl;
	printf("Type: %d ", icmp_protocol->type);
	switch (icmp_protocol->type)
	{
	case 8:
		printf("(request)\n");
		break;
	case 0:
		printf("(reply)\n");
		break;
	default:
		printf("\n");
		break;
	}
	printf("Code: %d\n", icmp_protocol->code);
	printf("CheckSum: 0x%.4x\n", ntohs(icmp_protocol->checksum));
	printf("Identification: 0x%.4x\n", ntohs(icmp_protocol->identification));
	printf("Sequence: 0x%.4x\n", ntohs(icmp_protocol->sequence));
	cout << endl;
}

void arp_protocol_packet_handle(
	u_char *arg,
	const struct pcap_pkthdr *pkt_header,
	const u_char *pkt_content
) {
	arp_header *arp_protocol;

	arp_protocol = (arp_header*)(pkt_content + 14);

	printf("==================ARP Protocol==================\n");
	cout << endl;
	printf("Hardware Type: ");
	switch (ntohs(arp_protocol->hardware_type))
	{
	case 1:
		printf("Ethernet");
		break;
	default:
		break;
	}
	printf(" (%d)\n", ntohs(arp_protocol->hardware_type));
	printf("Protocol Type: \n");
	switch (ntohs(arp_protocol->protocol_type))
	{
	case 0x0800:
		printf("%s", "IP");
		break;
	case 0x0806:
		printf("%s", "ARP");
		break;
	case 0x0835:
		printf("%s", "RARP");
		break;
	default:
		printf("%s", "Unknown Protocol");
		break;
	}
	printf(" (0x%04x)\n", ntohs(arp_protocol->protocol_type));
	printf("Hardware Length: %d\n", arp_protocol->hardware_length);
	printf("Protocol Length: %d\n", arp_protocol->protocol_length);
	printf("Operation Code: ");
	switch (ntohs(arp_protocol->operation_code))
	{
	case 1:
		printf("request");
		break;
	case 2:
		printf("reply");
		break;
	default:
		break;
	}
	printf(" (%i)\n", ntohs(arp_protocol->operation_code));
	cout << endl;
}



void IP_protocol_packet_handle(
	u_char *arg, 
	const struct pcap_pkthdr *pkt_header, 
	const u_char *pkt_content
) {
	ip_header *ip_protocol;
	sockaddr_in source, dest;
	char sourceIP[MAX_ADDR_LEN], destIP[MAX_ADDR_LEN];

	ip_protocol = (ip_header*)(pkt_content + 14);
	source.sin_addr.s_addr = ip_protocol->saddr;
	dest.sin_addr.s_addr = ip_protocol->daddr;
	strncpy(sourceIP, inet_ntoa(source.sin_addr), MAX_ADDR_LEN);
	strncpy(destIP, inet_ntoa(dest.sin_addr), MAX_ADDR_LEN);

	printf("===================IP Protocol==================\n");
	cout << endl;
	printf("Version: %d\n", ip_protocol->version_hlen >> 4);
	printf("Header Length: %d bytes\n", (ip_protocol->version_hlen & 0x0f) * 4);
	printf("Tos: %d\n", ip_protocol->tos);
	printf("Total Length: %d\n", ntohs(ip_protocol->tlen));
	
	printf("Identification: 0x%.4x (%i)\n", ntohs(ip_protocol->identification), ntohs(ip_protocol->identification));
	printf("Flags: %d\n", ntohs(ip_protocol->flags_offset) >> 13);
	printf("---Reserved bit: %d\n", (ntohs(ip_protocol->flags_offset) & 0x8000) >> 15);
	printf("---Don't fragment: %d\n", (ntohs(ip_protocol->flags_offset) & 0x4000) >> 14);
	printf("---More fragment: %d\n", (ntohs(ip_protocol->flags_offset) & 0x2000) >> 13);
	printf("Fragment offset: %d\n", ntohs(ip_protocol->flags_offset) & 0x1fff);
	printf("Time to live: %d\n", ip_protocol->ttl);
	printf("Protocol Type: ");
	switch (ip_protocol->proto)
	{
	case 1:
		printf("ICMP");
		break;
	case 6:
		printf("TCP");
		break;
	case 17:
		printf("UDP");
		break;
	default:
		break;
	}
	printf(" (%d)\n", ip_protocol->proto);
	printf("Header checkSum: 0x%.4x\n", ntohs(ip_protocol->checksum));
	printf("Source: %s\n", sourceIP);
	printf("Destination: %s\n", destIP);
	cout << endl;

	if (ip_protocol->proto == htons(0x0600))
		tcp_protocol_packet_handle(arg, pkt_header, pkt_content);
	else if (ip_protocol->proto == htons(0x1100))
		udp_protocol_packet_handle(arg, pkt_header, pkt_content);
	else if (ip_protocol->proto == htons(0x0100))
		icmp_protocol_packet_handle(arg, pkt_header, pkt_content);

}

void etherent_protocol_packet_handle(
	u_char *arg,
	const struct pcap_pkthdr *pkt_header,
	const u_char *pkt_content
) {
	ether_header *ethernet_protocol;//etherent protocol
	u_short ethernet_type;			//type of etherent
	u_char *mac_string;				//address of etherent

	//obtain data content of etherent
	ethernet_protocol = (ether_header*)pkt_content;
	ethernet_type = ntohs(ethernet_protocol->ether_type);

	printf("==============Ethernet Protocol=================\n");
	cout << endl;
	//target etherent address
	mac_string = ethernet_protocol->ether_dhost;

	printf("Destination Mac Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
		*mac_string,
		*(mac_string + 1),
		*(mac_string + 2),
		*(mac_string + 3),
		*(mac_string + 4),
		*(mac_string + 5));

	//source etherent address
	mac_string = ethernet_protocol->ether_shost;

	printf("Source Mac Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
		*mac_string,
		*(mac_string + 1),
		*(mac_string + 2),
		*(mac_string + 3),
		*(mac_string + 4),
		*(mac_string + 5));

	printf("Ethernet type: ");
	switch (ethernet_type)
	{
	case 0x0800:
		printf("%s", "IP");
		break;
	case 0x0806:
		printf("%s", "ARP");
		break;
	case 0x0835:
		printf("%s", "RARP");
		break;
	default:
		printf("%s", "Unknown Protocol");
		break;
	}
	printf(" (0x%04x)\n", ethernet_type);
	cout << endl;

	switch (ethernet_type)
	{
	case 0x0800:
		IP_protocol_packet_handle(arg, pkt_header, pkt_content);
		break;
	case 0x0806:
		arp_protocol_packet_handle(arg, pkt_header, pkt_content);
		break;
	case 0x0835:
		printf("==============RARP Protocol=================\n");
		printf("RARP\n");
		break;
	default:
		printf("==============Unknown Protocol==============\n");
		printf("Unknown Protocol\n");
		break;
	}
}

