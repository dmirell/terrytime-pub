import json
import websocket
import re
import requests
from threading import Thread, Lock

class MarketStream:
    _FLOAT_PRECISION = 2
    def __init__(self, symbol):
        lower_bound_symbol = symbol.lower()
        base_url = 'wss://stream.binance.com:9443'
        stream_url = f'{base_url}/ws/{lower_bound_symbol}@trade/{lower_bound_symbol}@depth@100ms'
        
        self._ws = websocket.WebSocketApp(stream_url, on_message=self.__on_message)
        self._market_stream_thread = Thread(target=self._ws.run_forever, daemon=True)
        self._market_stream_lock = Lock()
        self._market_stream_thread.start()
        self._market_stream_array = []

    def compare_with_terry_stream(self, data):
        terry_parsed = self.__parse_terry_data(data)
        terry_parsed = '\n'.join(terry_parsed)
        assert terry_parsed != '', "terry_parsed is empty"
        with self._market_stream_lock:
            market_stream_array_str = '\n'.join(self._market_stream_array)
            assert market_stream_array_str != '', "market_stream_array_str is empty"
            assert terry_parsed in market_stream_array_str, (terry_parsed, market_stream_array_str)

    def __on_message(self, _, message):
        data = json.loads(message)
        
        if 'e' in data and data['e'] == 'trade':
            with self._market_stream_lock:
                self._market_stream_array.append(self.__format_trade(data))

    def __format_trade(self, data):
        e_timestamp = data['E']
        t_timestamp = data['T']
        price = float(data['p'])
        quantity = float(data['q'])
        is_buyer_maker = int(data['m'])
        
        result = (
            f"Trade: [t_timestamp: {t_timestamp} ms, "
            f"price: {price:.{MarketStream._FLOAT_PRECISION}f}, "
            f"quantity: {quantity:.{MarketStream._FLOAT_PRECISION}f}, "
            f"is_buyer_maker: {is_buyer_maker}]"
        )
        return result
    
    def __parse_terry_data(self, data):
        result = []
        lines = data.split('\n')
        lines = lines[1:]

        # Process each line of data
        for line in lines:
            if line.startswith("Trade"):
                result.append(self.__parse_terry_trade_data(line))
        # print(result)
        return result

    def __parse_terry_trade_data(self, data):
        # Define a regular expression pattern to match the data
        pattern = r"Trade: \[e_timestamp: (\d+) ms, t_timestamp: (\d+) ms, r_timestamp: (\d+) ms, price: ([\d.]+), quantity: ([\d.]+), is_buyer_maker: (\d)\]"

        # Use re.match to extract groups from the pattern
        match = re.match(pattern, data)
        if match:
            e_timestamp, t_timestamp, r_timestamp, price, quantity, is_buyer_maker = match.groups()
            price = float(price)
            quantity = float(quantity)
            result = (
                f"Trade: [t_timestamp: {t_timestamp} ms, "
                f"price: {price:.{MarketStream._FLOAT_PRECISION}f}, "
                f"quantity: {quantity:.{MarketStream._FLOAT_PRECISION}f}, "
                f"is_buyer_maker: {is_buyer_maker}]"
            )
            return result
        return None
