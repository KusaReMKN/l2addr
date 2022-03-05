/*-
 * SPDX short identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 KusaReMKN.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/socket.h>

#include <net/if.h>
#if defined(__NetBSD__)
#include <net/if_ether.h>
#elif defined(__OpenBSD__)
#include <net/ethertypes.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#else /* !__NetBSD__ && !__OpenBSD__ */
#include <net/ethernet.h>
#endif /* __NetBSD__, __OpenBSD__ */
#ifdef __linux__
#include <linux/if_packet.h>
#include <netinet/ether.h>
#else /* !__linux__ */
#include <net/if_dl.h>
#include <net/if_types.h>
#endif /* __linux__ */

#include <err.h>
#include <ifaddrs.h>
#include <stdio.h>

int
main(void)
{
	struct ifaddrs *ifa, *res;
	struct ether_addr *ethaddr;

	if (getifaddrs(&res) == -1)
		err(1, "getifaddrs");
	for (ifa = res; ifa != NULL; ifa = ifa->ifa_next) {
#ifdef __linux__
		struct sockaddr_ll *sll;

		if (ifa->ifa_addr == NULL
				|| ifa->ifa_addr->sa_family != AF_PACKET)
			continue;
		sll = (struct sockaddr_ll *)ifa->ifa_addr;
		if (sll->sll_hatype != ARPHRD_ETHER
				|| sll->sll_halen != ETHER_ADDR_LEN)
			continue;
		ethaddr = (struct ether_addr *)sll->sll_addr;
#else /* !__linux__ */
		struct sockaddr_dl *sdl;

		if (ifa->ifa_addr == NULL
				|| ifa->ifa_addr->sa_family != PF_LINK)
			continue;
		sdl = (struct sockaddr_dl *)ifa->ifa_addr;
		if (sdl->sdl_type != IFT_ETHER
				|| sdl->sdl_alen != ETHER_ADDR_LEN)
			continue;
		ethaddr = (struct ether_addr *)LLADDR(sdl);
#endif /* __linux__ */
		printf("%s: %s\n", ifa->ifa_name, ether_ntoa(ethaddr));
	}
	freeifaddrs(res);

	return 0;
}
