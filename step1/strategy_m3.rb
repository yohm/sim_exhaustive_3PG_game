require 'pp'
require_relative 'state_m3'
require_relative 'strategy'

class StrategyM3

  def initialize( actions )
    raise "invalid arg" unless actions.all? {|act| act == :c or act == :d }
    raise "invalid arg" unless actions.size == 512
    @actions = actions.dup
  end

  def to_bits
    @actions.join('')
  end

  def show_actions(io)
    FullStateM3::NUM_STATES.times do |i|
      act = @actions[i]
      stat = FullStateM3.make_from_id(i)
      io.print "#{act}|#{stat}\t"
      io.print "\n" if i % 10 == 9
    end
    io.print "\n"
  end

  def self.make_from_bits( bits )
    actions = bits.each_char.map do |chr|
      chr.to_sym
    end
    self.new( actions )
  end

  def self.make_from_m2_strategy( m2_stra )
    acts = []
    FullStateM3::NUM_STATES.times do |i|
      m3_stat = FullStateM3.make_from_id(i)
      m2_stat = m3_stat.to_m2_states.last
      act = m2_stra.action( m2_stat.to_ss )
      acts << act
    end
    self.new(acts)
  end

  def modify_action( state, action )
    if state.is_a?(String)
      stat = FullStateM3.make_from_bits(state)
      raise "action does not change: #{stat}" if @actions[stat.to_id] == action
      @actions[stat.to_id] = action
    elsif state.is_a?(FullStateM3)
      @actions[state.to_id] = action
    else
      raise "invalid arg"
    end
  end

  def action( state_id )
    @actions[state_id]
  end

  def valid?
    @actions.all? {|a| a == :c or a == :d }
  end

  def possible_next_full_states(current_fs)
    sid = current_fs.to_id
    act_a = action(sid)
    n1 = current_fs.next_state(act_a,:c,:c)
    n2 = current_fs.next_state(act_a,:c,:d)
    n3 = current_fs.next_state(act_a,:d,:c)
    n4 = current_fs.next_state(act_a,:d,:d)
    [n1,n2,n3,n4]
  end

  def defensible?
    a1_b, a1_c = AMatrix.construct_a1_matrix(self) # construct_a1_matrix
    a_b, a_c = AMatrix.construct_a1_matrix(self)
    return false if( a_b.has_negative_diagonal? or a_c.has_negative_diagonal? )

    FullStateM3::NUM_STATES.times do |t|
      a_b.update( a1_b )
      a_c.update( a1_c )
      return false if( a_b.has_negative_diagonal? or a_c.has_negative_diagonal? )
    end
    true
  end

  class AMatrix  # class used for judging defensibility

    N = FullStateM3::NUM_STATES

    def self.construct_a1_matrix(stra)
      a_b = self.new
      a_c = self.new

      N.times do |i|
        fs = FullStateM3.make_from_id(i)
        N.times do |j|
          a_b.a[i][j] = :inf
          a_c.a[i][j] = :inf
        end
        next_fss = stra.possible_next_full_states(fs)
        next_fss.each do |ns|
          j = ns.to_id
          a_b.a[i][j] = ns.relative_payoff_against(:B)
          a_c.a[i][j] = ns.relative_payoff_against(:C)
        end
      end
      [a_b, a_c]
    end

    attr_reader :a

    def initialize
      @a = Array.new(N) {|i| Array.new(N,0) }
    end

    def inspect
      sio = StringIO.new
      @a.size.times do |i|
        @a[i].size.times do |j|
          if @a[i][j] == :inf
            sio.print(" ##,")
          else
            sio.printf("%3d,", @a[i][j])
          end
        end
        sio.print "\n"
      end
      sio.string
    end

    def has_negative_diagonal?
      @a.size.times do |i|
        if @a[i][i] != :inf and @a[i][i] < 0
          return true
        end
      end
      false
    end

    def update( a1 )
      temp = Array.new(N) {|i| Array.new(N,:inf) }

      N.times do |i|
        N.times do |j|
          N.times do |k|
            x = @a[i][k]
            y = a1.a[k][j]
            next if x == :inf or y == :inf
            temp[i][j] = x+y if temp[i][j] == :inf or x+y < temp[i][j]
          end
        end
      end
      @a = temp
    end
  end

end

if __FILE__ == $0
  if ARGV.size == 1
    bits = ARGV[0]
    stra = Strategy.make_from_bits(bits)
    stra.show_actions($stdout)
    a1_b, a1_c = Strategy::AMatrix.construct_a1_matrix(stra)
    pp a1_b
    pp a1_b.has_negative_diagonal?
    a1_b.update(a1_b)
    pp a1_b
    pp "def: #{stra.defensible?}"

    exit 0
  end

  bits = "cddd" * 128
  strategy = StrategyM3.make_from_bits(bits)
  strategy.show_actions($stdout)
  pp strategy.valid?, strategy.defensible?
  raise "inconsistent bits" unless bits == strategy.to_bits

  bits = "cddcddccddcdddcdddddddccdddcccccdddddddd"
  m2_stra = Strategy.make_from_bits(bits)
  m3_stra = StrategyM3.make_from_m2_strategy(m2_stra)

  # noise on B&C
  m3_stra.modify_action('ccdccdccc',:c)
  m3_stra.modify_action('ccdcccccd',:c)
  m3_stra.modify_action('cdccdcccd',:c)
  m3_stra.modify_action('cdcccdcdc',:c)
  # noise on B -> on B
  m3_stra.modify_action('cddccdccd',:c)
  m3_stra.modify_action('cddddccdd',:c)
  m3_stra.modify_action('cddcddddc',:c)
  # noise on B -> on C
  m3_stra.modify_action('cdcccdccc',:c)
  m3_stra.modify_action('cdccccccd',:c)
  m3_stra.modify_action('ccccdcccd',:c)
  m3_stra.modify_action('cccccdcdc',:c)
  # noise on B -> _ -> on B
  m3_stra.modify_action('dcdcdccdc',:c)
  # noise on B -> _ -> on C
  m3_stra.modify_action('cdddcccdc',:c)
  m3_stra.modify_action('cddcdcdcc',:c)


  m3_stra.show_actions($stdout)
  puts m3_stra.to_bits
end
