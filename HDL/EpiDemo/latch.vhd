library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity latch is
	generic(
		N: integer
	);
	port(
		clock : in  std_logic;
		inData : in  std_logic_vector (N - 1 downto 0);
		outData : out  std_logic_vector (N - 1 downto 0)
	);
end latch;

architecture Behavioral of latch is

	signal r, rin : std_logic_vector(N - 1 downto 0);

begin

	combinational : process(inData, r)
	
	begin
		rin <= inData;
		outData <= r;
	end process;

	sequential : process(clock, rin)
	begin
		if rising_edge(clock) then 
			r <= rin;
		end if;
	end process;

end Behavioral;
