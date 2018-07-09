using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;

namespace conct
{
	public static class CollectionExtensions
	{
		public static void Sync<TTarget, TSource>(this ICollection<TTarget> targets, IEnumerable<TSource> sources, Func<TSource, TTarget, bool> compareFunc, Func<TSource, TTarget> createFunc)
		{
			List<TTarget> targetsToRemove = new List<TTarget>();
			foreach (TTarget target in targets)
			{
				if (!sources.Any(s => compareFunc(s, target)))
				{
					targetsToRemove.Add(target);
				}
			}

			foreach (TTarget target in targetsToRemove)
			{
				targets.Remove(target);
			}

			foreach (TSource source in sources)
			{
				if (!targets.Any(t => compareFunc(source, t)))
				{
					targets.Add(createFunc(source));
				}
			}
		}
	}
}