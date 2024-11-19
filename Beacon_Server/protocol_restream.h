int callback_beacon_restream(struct lws*, enum lws_callback_reasons, void *, void *, size_t);

/* here we put all the abpve into a nice neat package that the server program can include in its setup */
#define LWS_PLUGIN_PROTOCOL_BEACON_RESTREAM \
	{                                       \
		"Beacon-Restream",                  \
		callback_beacon_restream,           \
		1280,                               \
		4097,                               \
		0, NULL, 0                          \
	}


