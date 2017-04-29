library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity latch8 is
	port
	(
		clock : in  std_logic;
		inData : in  std_logic_vector (7 downto 0);
		outData : out  std_logic_vector (7 downto 0)
	);
end latch8;

architecture Behavioral of latch8 is

	signal r, rin : std_logic_vector( 7 downto 0);

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
