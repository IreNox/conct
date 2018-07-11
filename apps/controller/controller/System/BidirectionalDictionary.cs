using System;
using System.Collections.Generic;

namespace conct
{
	public class BidirectionalDictionary<T1, T2>
	{
		public class Indexer<T3, T4>
		{
			private Dictionary<T3, T4> m_dictionary;

			public Indexer(Dictionary<T3, T4> dictionary)
			{
				m_dictionary = dictionary;
			}

			public T4 this[T3 index]
			{
				get { return m_dictionary[index]; }
			}
		}

		private Dictionary<T1, T2> m_forward = new Dictionary<T1, T2>();
		private Dictionary<T2, T1> m_reverse = new Dictionary<T2, T1>();

		public BidirectionalDictionary()
		{
			this.Forward = new Indexer<T1, T2>(m_forward);
			this.Reverse = new Indexer<T2, T1>(m_reverse);
		}

		public void Add(T1 t1, T2 t2)
		{
			m_forward.Add(t1, t2);
			m_reverse.Add(t2, t1);
		}

		public bool Contains1(T1 t1)
		{
			return m_forward.ContainsKey(t1);
		}

		public bool Contains2(T2 t2)
		{
			return m_reverse.ContainsKey(t2);
		}

		public Indexer<T1, T2> Forward { get; private set; }
		public Indexer<T2, T1> Reverse { get; private set; }
	}
}
