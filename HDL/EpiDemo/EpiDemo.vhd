library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity EpiDemo is

	port(
		EPI0S : in std_logic_vector(31 downto 0);
		LED : out std_logic_vector(7 downto 0)
	);

end entity;

architecture Structural of EpiDemo is

	component latch8 is
		port(
			clock : in  std_logic;
			inData : in  std_logic_vector (7 downto 0);
			outData : out  std_logic_vector (7 downto 0)
		);
	end component;
	
begin

	latch : latch8
		port map
		(
			clock => EPI0S(31),
			inData(7 downto 0) => EPI0S(7 downto 0),
			outData(7 downto 0) => LED(7 downto 0)
		);

end architecture;
